# ReSTIR DI Temporal + Spatial Reuse 验证记录

本文记录 GPU/OptiX backend 中 ReSTIR DI temporal reuse 与 spatial reuse 的实现范围、关键修正、实验设置和当前结论。它是 `docs/restir_no_reuse_validation.md` 的后续阶段。

## 当前实现范围

GPU direct lighting 现在支持：

```text
--direct-light nee
--direct-light restir
--restir-initial-candidates N
--restir-temporal 0|1
--restir-max-history N
--restir-spatial 0|1
--restir-spatial-samples N
--restir-spatial-radius R
```

Temporal reuse 复用上一帧 reservoir，并通过 primary hit reprojection、material id、normal agreement、world-space hit position 和 target ratio 做 rejection。

Spatial reuse 使用两次 OptiX launch：

1. 第一遍生成稳定的 same-frame source reservoirs 和 surface data。
2. 第二遍读取 source buffers，采样邻居 reservoir，重新在当前 shading point 评估 target，并将结果写入 spatial output reservoir。

如果 beauty pass 使用 spatial reuse，spatial output reservoir 会作为下一帧 temporal history 的当前帧结果。

## Debug Views

ReSTIR 相关 debug views 包括：

```text
reservoir-weight
reservoir-m
reservoir-target
restir-light-id
prev-restir-light-id
temporal-candidate-target
temporal-target-ratio
temporal-accepted
temporal-source
temporal-reproject-valid
spatial-accepted
spatial-source
spatial-neighbor-offset
spatial-target-ratio
```

这些 debug views 用于确认 temporal reprojection、spatial neighbor acceptance、reservoir weight/M 和 selected light id 是否符合预期。

## 关键实现修正

本轮实现过程中修过几个会影响结论的问题：

- Temporal merge 的 represented candidate 权重改为使用 previous reservoir 的 final weight，使 temporal reuse 不再系统性偏亮。
- Temporal debug views 改为使用 reprojected previous pixel，而不是 same-pixel history。
- Spatial beauty pass 改为两遍 launch，避免第二遍覆盖第一遍 source reservoir。
- Spatial second pass 不再强制 `maxBounces = 1`，保证 full-depth beauty path 不被意外截断。
- Raygen 在 spatial pass 中不再清空 source reservoirs，避免 spatial beauty 变暗。
- Screen-space ReSTIR reservoir 只用于 primary hit；secondary bounces 使用 local no-reuse ReSTIR estimator，避免 secondary hit 读写 primary pixel history。
- Path RNG 和 direct-light/ReSTIR RNG 拆分，避免 different estimator paths 改变 BSDF continuation random stream。
- Spatial neighbor acceptance 收紧了 normal agreement、plane distance、world-space distance 和 target ratio，降低跨几何边界复用的风险。

## Many-Light Showcase Scene

本轮验证主要使用 Sponza many-lights 场景：

```text
assets/validation/sponza_many_tiny_lights_64.xml
```

该场景使用 split-by-material 的 Sponza OBJ 子网格，并用轻量 diffuse color 代替 texture IO。灯光为 64 个小 area lights，用于放大 direct-light sampling 难度。

## Direct Lighting 对比实验

为了验证 ReSTIR DI 本身，最终采用 direct-light-only 实验：

```text
scene: assets/validation/sponza_many_tiny_lights_64.xml
resolution: 512x288
frames: 120
camera: fixed orbit sequence, 8 degrees
max-depth: 1
accumulation: disabled
denoiser: disabled
reference: NEE, spp=1024
```

推荐展示设置：

```text
spp = 4
restir-initial-candidates = 4
spatial samples = 5
spatial radius = 16 pixels
```

输出路径：

```text
build-gpu/gpu/RelWithDebInfo/comparison_di_k4_spp4_spp16/spp4
```

平均 120 帧结果如下：

| Method | Mean RGB MAE | Mean RGB RMSE | Mean PSNR | Mean Luma Ratio |
| --- | ---: | ---: | ---: | ---: |
| NEE | 0.2760 | 0.3761 | 8.50 dB | 0.5852 |
| ReSTIR no-reuse | 0.1919 | 0.2723 | 11.30 dB | 0.9263 |
| ReSTIR temporal + spatial | 0.1827 | 0.2611 | 11.67 dB | 1.0505 |

逐帧 PSNR winner：

```text
ReSTIR temporal + spatial: 119 / 120 frames
ReSTIR no-reuse:             1 / 120 frames
NEE:                         0 / 120 frames
```

该设置满足期望趋势：

```text
PSNR: NEE < ReSTIR no-reuse < ReSTIR temporal + spatial
```

## Low-Candidate Sanity Check

在 `spp=4, initialCandidates=1, max-depth=1` 下，no-reuse ReSTIR 与 NEE 几乎一致，这是预期行为：只有一个 candidate 时，no-reuse reservoir estimator 基本退化为 one-sample NEE。

同一设置下 temporal + spatial reuse 明显优于 NEE/no-reuse：

| Method | Mean RGB MAE | Mean RGB RMSE | Mean PSNR | Mean Luma Ratio |
| --- | ---: | ---: | ---: | ---: |
| NEE | 0.2760 | 0.3761 | 8.50 dB | 0.5852 |
| ReSTIR no-reuse, k=1 | 0.2764 | 0.3768 | 8.48 dB | 0.5853 |
| ReSTIR temporal + spatial, k=1 | 0.1838 | 0.2629 | 11.61 dB | 1.0181 |

逐帧 MAE winner：

```text
ReSTIR temporal + spatial: 120 / 120 frames
```

## Full-Path Showcase Observation

在 full-depth path tracing、较强 baseline 设置下：

```text
spp = 16
restir-initial-candidates = 32
max-depth = 8
```

no-reuse ReSTIR 的单帧质量已经很强，temporal/spatial reuse 的视觉优势不稳定，并且可能出现更明显的 correlated noise。该实验不适合用来声称 temporal + spatial 在当前实现中一定优于 no-reuse。

对应 120 帧平均结果：

| Method | Mean RGB MAE | Mean RGB RMSE | Mean PSNR | Mean Luma Ratio |
| --- | ---: | ---: | ---: | ---: |
| NEE | 0.1655 | 0.2217 | 13.08 dB | 0.8693 |
| ReSTIR no-reuse, k=32 | 0.1308 | 0.1712 | 15.34 dB | 1.1355 |
| ReSTIR temporal + spatial, k=32, s4 r16 | 0.1509 | 0.1931 | 14.29 dB | 1.0994 |
| ReSTIR temporal + spatial, k=32, s1 r4 | 0.1587 | 0.2011 | 13.93 dB | 1.0832 |

结论：当前实现更适合用 direct-light validation 展示 temporal/spatial reuse 的收益；full-path production-quality temporal/spatial reuse 仍需要更完整的 bias handling、visibility handling 和 filtering。

## 当前结论

当前可以认为完成了：

```text
Done: ReSTIR DI temporal + spatial reuse prototype and direct-light validation
```

更谨慎地说，它是一个 validation milestone，而不是 production renderer。它已经能在 low-candidate direct-light many-light 场景中复现预期趋势，但在强 no-reuse baseline 和 full-path 场景下仍有局限。

## 后续工作

建议后续优先处理：

1. 更严格的 temporal disocclusion / visibility validation。
2. Spatial reuse 的更完整 bias correction、Jacobian / reconnection term 和 MIS 权重处理。
3. 对 temporal/spatial correlated noise 增加 clamp 或 filtering 策略。
4. 添加 HDR/EXR 输出，避免基于 tone-mapped PNG 的指标受 clamp/gamma 影响。
5. 将 direct-light validation 固化为可复现脚本，减少手动命令和一次性输出目录。
