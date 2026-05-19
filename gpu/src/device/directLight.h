// ======================================================================== //
// directLight.h - device-side NEE and no-reuse ReSTIR DI helpers.
// ======================================================================== //

#pragma once

#include "pt/math/vec.h"
#include "reprojection.h"
#include "shading/bsdf.h"
#include "device/prd.h"
#include "device/visibility.h"
#include "pod/launchParams.h"
#include "pod/light.h"
#include "ptInterop.h"
#include "pt/restir/reservoir.h"
#include "pt/restir/target.h"
#include "pod/restirState.h"
#include "shading/utils.h"

using namespace owl;
using namespace pt;

__device__ inline bool generateDirectLightCandidate(
  const LaunchParams &params,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  float uLight,
  vec2f uSurface,
  pt::RestirDirectLightCandidate &out);

__device__ inline bool evaluateReservoirSampleAtCurrentHit(
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  const pt::RestirLightSample &sample,
  pt::RestirDirectLightCandidate &out);

__device__ inline bool acceptTemporalReservoirCandidate(
  const LaunchParams &params,
  int pxIdx,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  pt::RestirDirectLightCandidate &outTemporalCandidate,
  pt::RestirReservoir &outPrevReservoir);

__device__ inline bool sampleSpatialNeighborPixel(const LaunchParams &params,
                                                  int pxIdx,
                                                  float uAngle,
                                                  float uRadius,
                                                  int &outNeighborPxIdx,
                                                  vec2f &outNormalizedOffset);

__device__ inline bool acceptSpatialReservoirCandidate(
  const LaunchParams &params,
  int pxIdx,
  int neighborPxIdx,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  pt::RestirDirectLightCandidate &outSpatialCandidate,
  pt::RestirReservoir &outNeighborReservoir);

__device__ inline bool mergeSpatialReservoirCandidates(
  const LaunchParams &params,
  int pxIdx,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  pt::RestirReservoir &reservoir,
  pt::RestirDirectLightCandidate &selectedCandidate,
  int &selectedSource,
  RNG &rng);

// Re-evaluate a previous-frame reservoir sample at the current hit point.
__device__ inline bool evaluateReservoirSampleAtCurrentHit(
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  const pt::RestirLightSample &sample,
  pt::RestirDirectLightCandidate &out)
{
  out = pt::RestirDirectLightCandidate();

  const vec3f lightP = fromPtVec(sample.position);
  const vec3f lightN = fromPtVec(sample.normal);
  const vec3f lightLe = fromPtVec(sample.emission);

  const vec3f toLight = lightP - prd.hitP;
  const float dist2 = dot(toLight, toLight);
  if (dist2 <= 1e-7f || sample.sourcePdf <= 0.f)
    return false;

  const vec3f wi = normalize(toLight);

  const float NoI = fmaxf(dot(prd.N, wi), 0.f);
  const float NoL = fmaxf(dot(lightN, -wi), 0.f);
  if (NoI <= 0.f || NoL <= 0.f)
    return false;

  const vec3f f = bsdf.f(wo, wi);
  const float G = NoI * NoL / dist2;
  const vec3f unshadowedContribution = lightLe * f * G;
  const float target = pt::restirTargetFromRgb(toPtVec(unshadowedContribution));
  if (target <= 0.f)
    return false;

  out.sample = sample;
  out.sample.target = target;
  out.wi = toPtVec(wi);
  out.unshadowedContribution = toPtVec(unshadowedContribution);
  return true;
}

// Fetch the temporal candidate from the previous frame and check if it is valid.
__device__ inline bool acceptTemporalReservoirCandidate(
  const LaunchParams &params,
  int pxIdx,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  pt::RestirDirectLightCandidate &outTemporalCandidate,
  pt::RestirReservoir &outPrevReservoir)
{
  outTemporalCandidate = pt::RestirDirectLightCandidate();
  outPrevReservoir = pt::RestirReservoir();

  // This is same-pixel history validation only. It is useful for static-camera
  // plumbing and debug views, but camera motion still needs real reprojection.
  if (!params.restirTemporal ||
      !params.prevRestirReservoirs ||
      !params.prevRestirSurfaceData) {
    return false;
  }

  int prevPxIdx = -1;
  reprojectCurrentHitToPreviousPixel(params, prd, prevPxIdx);
  if (prevPxIdx < 0) {
    return false;
  }
  const pt::RestirReservoir prevReservoir = params.prevRestirReservoirs[prevPxIdx];
  if (prevReservoir.M == 0 ||
      prevReservoir.W <= 0.f ||
      prevReservoir.y.target <= 0.f) {
    return false;
  }

  const pt::RestirSurfaceData prevSurface = params.prevRestirSurfaceData[prevPxIdx];
  if (!prevSurface.valid || !prd.didHit) {
    return false;
  }

  if (prevSurface.materialId != prd.materialId) {
    return false;
  }

  // Same-pixel history should still describe the same local surface.
  const float normalAgreement = dot(prevSurface.normal, prd.N);
  if (normalAgreement < 0.9f) {
    return false;
  }

  const vec3f surfaceDelta = prevSurface.hitP - prd.hitP;
  const float surfaceDist2 = dot(surfaceDelta, surfaceDelta);
  if (surfaceDist2 > 0.01f) {
    return false;
  }

  if (!evaluateReservoirSampleAtCurrentHit(prd,
                                           bsdf,
                                           wo,
                                           prevReservoir.y,
                                           outTemporalCandidate)) {
    return false;
  }

  // Reject samples whose importance changes too drastically at the current hit.
  const float targetRatio =
    outTemporalCandidate.sample.target / prevReservoir.y.target;
  if (targetRatio < 0.1f || targetRatio > 10.f) {
    return false;
  }

  outPrevReservoir = prevReservoir;
  return true;
}

// Pick a random neighbor pixel inside the configured spatial radius.
//
// This is intentionally only the neighbor sampling scaffold. The reservoir
// candidate itself is read from spatialSource* buffers generated by a prior
// launch, not from the buffer currently being written.
__device__ inline bool sampleSpatialNeighborPixel(const LaunchParams &params,
                                                  int pxIdx,
                                                  float uAngle,
                                                  float uRadius,
                                                  int &outNeighborPxIdx,
                                                  vec2f &outNormalizedOffset)
{
  outNeighborPxIdx = -1;
  outNormalizedOffset = vec2f(0.f);

  if (!params.restirSpatial ||
      params.fbSize.x <= 0 ||
      params.fbSize.y <= 0 ||
      params.restirSpatialRadius <= 0) {
    return false;
  }

  const int x = pxIdx % params.fbSize.x;
  const int y = pxIdx / params.fbSize.x;
  const float radius = float(params.restirSpatialRadius) * sqrtf(fmaxf(uRadius, 0.f));
  const float angle = 6.28318530718f * uAngle;
  int dx = int(cosf(angle) * radius);
  int dy = int(sinf(angle) * radius);

  if (dx == 0 && dy == 0) {
    dx = 1;
  }

  const int nx = min(max(x + dx, 0), params.fbSize.x - 1);
  const int ny = min(max(y + dy, 0), params.fbSize.y - 1);
  if (nx == x && ny == y) {
    return false;
  }

  outNeighborPxIdx = ny * params.fbSize.x + nx;
  outNormalizedOffset = vec2f(float(nx - x), float(ny - y)) /
                        float(params.restirSpatialRadius);
  return true;
}

// Validate a neighbor reservoir sample as a spatial candidate for the current
// shading point.
//
// This reads the stable source reservoir generated by the first spatial debug
// launch. It validates only the candidate; actual spatial merging is still a
// TODO so beauty output remains unchanged.
__device__ inline bool acceptSpatialReservoirCandidate(
  const LaunchParams &params,
  int pxIdx,
  int neighborPxIdx,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  pt::RestirDirectLightCandidate &outSpatialCandidate,
  pt::RestirReservoir &outNeighborReservoir)
{
  (void)pxIdx;
  outSpatialCandidate = pt::RestirDirectLightCandidate();
  outNeighborReservoir = pt::RestirReservoir();

  if (!params.restirSpatial ||
      !params.spatialSourceReservoirs ||
      !params.spatialSourceSurfaceData ||
      neighborPxIdx < 0 ||
      neighborPxIdx >= params.fbSize.x * params.fbSize.y) {
    return false;
  }

  const pt::RestirReservoir neighborReservoir =
    params.spatialSourceReservoirs[neighborPxIdx];
  if (neighborReservoir.M == 0 ||
      neighborReservoir.W <= 0.f ||
      neighborReservoir.y.target <= 0.f) {
    return false;
  }

  const pt::RestirSurfaceData neighborSurface =
    params.spatialSourceSurfaceData[neighborPxIdx];
  if (!neighborSurface.valid || !prd.didHit) {
    return false;
  }

  if (neighborSurface.materialId != prd.materialId) {
    return false;
  }

  const float normalAgreement = dot(neighborSurface.normal, prd.N);
  if (normalAgreement < 0.97f) {
    return false;
  }

  // Spatial reuse is very sensitive to mixing across geometric edges. Keep this
  // conservative until the merge has proper MIS/bias correction.
  const vec3f surfaceDelta = neighborSurface.hitP - prd.hitP;
  const float surfaceDist2 = dot(surfaceDelta, surfaceDelta);
  const float planeDistance = fabsf(dot(surfaceDelta, prd.N));
  if (surfaceDist2 > 0.25f || planeDistance > 0.03f) {
    return false;
  }

  if (!evaluateReservoirSampleAtCurrentHit(prd,
                                           bsdf,
                                           wo,
                                           neighborReservoir.y,
                                           outSpatialCandidate)) {
    return false;
  }

  const float targetRatio =
    outSpatialCandidate.sample.target / neighborReservoir.y.target;
  if (targetRatio < 0.25f || targetRatio > 4.f) {
    return false;
  }

  outNeighborReservoir = neighborReservoir;
  return true;
}

// Merge accepted neighbor reservoirs into the current pixel reservoir.
//
// This helper is meant for the second spatial pass only:
// - `reservoir` starts as the current pixel's source reservoir from Pass A.
// - accepted neighbors are re-evaluated at the current hit point before merge.
// - replacement from a neighbor marks selectedSource = 3 for spatial-source.
__device__ inline bool mergeSpatialReservoirCandidates(
  const LaunchParams &params,
  int pxIdx,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  pt::RestirReservoir &reservoir,
  pt::RestirDirectLightCandidate &selectedCandidate,
  int &selectedSource,
  RNG &rng)
{
  if (!params.restirSpatial || !params.restirSpatialPass) {
    return false;
  }

  bool acceptedAny = false;
  for (int i = 0; i < params.restirSpatialSamples; ++i) {
    int neighborPxIdx = -1;
    vec2f normalizedOffset(0.f);
    if (!sampleSpatialNeighborPixel(params,
                                    pxIdx,
                                    rng(),
                                    rng(),
                                    neighborPxIdx,
                                    normalizedOffset)) {
      continue;
    }

    pt::RestirDirectLightCandidate spatialCandidate;
    pt::RestirReservoir neighborReservoir;
    if (!acceptSpatialReservoirCandidate(params,
                                         pxIdx,
                                         neighborPxIdx,
                                         prd,
                                         bsdf,
                                         wo,
                                         spatialCandidate,
                                         neighborReservoir)) {
      continue;
    }

    acceptedAny = true;
    const uint32_t representedM =
      min(neighborReservoir.M, uint32_t(params.restirMaxHistory));
    const bool replaced = pt::updateReservoirWithRepresentedCandidate(
      reservoir,
      spatialCandidate.sample,
      spatialCandidate.sample.target,
      neighborReservoir.W,
      representedM,
      rng());
    if (replaced) {
      selectedCandidate = spatialCandidate;
      selectedSource = 3;
    }
  }

  return acceptedAny;
}

__device__ inline vec3f estimateDirectLightNee(
  const LaunchParams &params,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  float uLight,
  vec2f uSurface)
{
  LightSample lightSample;
  if (sampleLight(params.lights, params.lightCount, uLight, uSurface, lightSample)) {
    vec3f toLight = lightSample.p - prd.hitP;
    float dist2 = dot(toLight, toLight);

    if (dist2 > 1e-7f && lightSample.pdfA > 0.f) {
      float dist = sqrtf(dist2);
      vec3f wi = toLight * (1.f / dist);

      float NoI = fmaxf(dot(prd.N, wi), 0.f);
      float NoL = fmaxf(dot(lightSample.n, -wi), 0.f);

      if (NoI > 0.f && NoL > 0.f) {
        const vec3f V = traceVisibility(params.world, prd.hitP, lightSample.p);
        if (V.x > 0.f || V.y > 0.f || V.z > 0.f) {
          const vec3f f = bsdf.f(wo, wi);
          const float G = NoI * NoL / dist2;
          return lightSample.Le * f * V * G / lightSample.pdfA;
        }
      }
    }
  }
  return vec3f(0.f);
}

__device__ inline vec3f estimateDirectLightReservoir(
  const LaunchParams &params,
  int pxIdx,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  RNG &rng)
{
  pt::RestirReservoir reservoir;
  pt::RestirDirectLightCandidate selectedCandidate;
  int selectedSource = 0; // 0: none, 1: current, 2: temporal, 3: spatial

  if (params.restirSpatialPass && params.spatialSourceReservoirs) {
    reservoir = params.spatialSourceReservoirs[pxIdx];
    if (params.restirSelectionSources) {
      selectedSource = params.restirSelectionSources[pxIdx];
    }
    if (reservoir.M > 0 &&
        reservoir.W > 0.f &&
        reservoir.y.target > 0.f &&
        !evaluateReservoirSampleAtCurrentHit(prd,
                                             bsdf,
                                             wo,
                                             reservoir.y,
                                             selectedCandidate)) {
      pt::clearReservoir(reservoir);
      selectedSource = 0;
    }
  } else {
  // No-reuse ReSTIR DI, local reservoir only:
  // 1. Generate N initial candidates from the current light sampler.
  // 2. Feed each valid candidate into reservoir update.
  // 3. Keep a copy of the candidate when updateReservoir() replaces y.
  //
  // This is where --restir-initial-candidates is consumed on the device.
  // For each frame, generate N initial candidates and update the local reservoir.
  for (int i = 0; i < params.restirInitialCandidates; ++i) {
    pt::RestirDirectLightCandidate candidate;
    const bool validCandidate =
      generateDirectLightCandidate(params,
                                   prd,
                                   bsdf,
                                   wo,
                                   rng(),
                                   vec2f(rng(), rng()),
                                   candidate);
    if (!validCandidate)
      continue;

    // TODO(ReSTIR): update the local reservoir with this candidate.
    const bool replaced =
      pt::updateReservoir(reservoir, candidate.sample, rng());
    if (replaced) {
      selectedCandidate = candidate;
      selectedSource = 1;
    }
  }

  // temporal merge reservoir
  pt::RestirDirectLightCandidate temporalCandidate;
  pt::RestirReservoir prevReservoir;
  if (acceptTemporalReservoirCandidate(params,
                                       pxIdx,
                                       prd,
                                       bsdf,
                                       wo,
                                       temporalCandidate,
                                       prevReservoir))
  {
    const uint32_t representedM =
      min(prevReservoir.M, uint32_t(params.restirMaxHistory));
    
      const bool replaced = pt::updateReservoirWithRepresentedCandidate(
        reservoir,
        temporalCandidate.sample,
        temporalCandidate.sample.target,
        prevReservoir.W,
        representedM,
        rng());
      if (replaced) {
        selectedCandidate = temporalCandidate;
        selectedSource = 2;
      }
  }
  }

  if (params.restirSpatialPass) {
    mergeSpatialReservoirCandidates(params,
                                    pxIdx,
                                    prd,
                                    bsdf,
                                    wo,
                                    reservoir,
                                    selectedCandidate,
                                    selectedSource,
                                    rng);
  }

  pt::finalizeReservoir(reservoir);
  if (params.restirSelectionSources) {
    params.restirSelectionSources[pxIdx] =
      (reservoir.W > 0.f && reservoir.y.target > 0.f) ? selectedSource : 0;
  }

  // Write to buffer, spatial pass will always write to spatial output reservoirs.
  // When a spatial pass is used, write to spatialOutputReservoirs
  // Otherwise, we write to current Reservoir (both no reuse and temporal reuse only)
  if (params.restirSpatialPass && params.spatialOutputReservoirs) {
    params.spatialOutputReservoirs[pxIdx] = reservoir;
  } else if (params.restirReservoirs) {
    params.restirReservoirs[pxIdx] = reservoir;
  }

  // trace visibility only for the selected reservoir sample.
  if (reservoir.W > 0.f && reservoir.y.target > 0.f) {
    const vec3f lightP = fromPtVec(selectedCandidate.sample.position);
    const vec3f V = traceVisibility(params.world, prd.hitP, lightP);
    if (V.x > 0.f || V.y > 0.f || V.z > 0.f) {
      return fromPtVec(selectedCandidate.unshadowedContribution) * V * reservoir.W;
    }
  }

  return vec3f(0.f);
};

__device__ inline vec3f estimateDirectLightReservoirLocal(
  const LaunchParams &params,
  const PRD &prd,
  const BSDF &bsdf,
  const vec3f &wo,
  RNG &rng)
{
  pt::RestirReservoir reservoir;
  pt::RestirDirectLightCandidate selectedCandidate;

  for (int i = 0; i < params.restirInitialCandidates; ++i) {
    pt::RestirDirectLightCandidate candidate;
    if (!generateDirectLightCandidate(params,
                                      prd,
                                      bsdf,
                                      wo,
                                      rng(),
                                      vec2f(rng(), rng()),
                                      candidate)) {
      continue;
    }

    if (pt::updateReservoir(reservoir, candidate.sample, rng())) {
      selectedCandidate = candidate;
    }
  }

  pt::finalizeReservoir(reservoir);
  if (reservoir.W > 0.f && reservoir.y.target > 0.f) {
    const vec3f lightP = fromPtVec(selectedCandidate.sample.position);
    const vec3f V = traceVisibility(params.world, prd.hitP, lightP);
    if (V.x > 0.f || V.y > 0.f || V.z > 0.f) {
      return fromPtVec(selectedCandidate.unshadowedContribution) * V * reservoir.W;
    }
  }

  return vec3f(0.f);
}

__device__ inline void storeRestirSurfaceData(const LaunchParams &params,
                                             int pxIdx,
                                             const PRD &prd)
{
  if (!params.restirSurfaceData) return;

  RestirSurfaceData surface;
  if (prd.didHit) {
    surface.hitP = prd.hitP;
    surface.normal = prd.N;
    surface.materialId = prd.materialId;
    surface.valid = 1;
  }

  // TODO(ReSTIR temporal): store depth or previous-frame projection data if
  // world-space hitP rejection is not stable enough for camera motion.
  params.restirSurfaceData[pxIdx] = surface;
}

__device__ inline bool generateDirectLightCandidate(const LaunchParams &params,
                                                    const PRD &prd,
                                                    const BSDF &bsdf,
                                                    const vec3f &wo,
                                                    float uLight,
                                                    vec2f uSurface,
                                                    pt::RestirDirectLightCandidate &out)
{
  out = pt::RestirDirectLightCandidate();

  LightSample lightSample;
  if (!sampleLight(params.lights, params.lightCount, uLight, uSurface, lightSample))
    return false;

  const vec3f toLight = lightSample.p - prd.hitP;
  const float dist2 = dot(toLight, toLight);
  if (dist2 <= 1e-7f || lightSample.pdfA <= 0.f)
    return false;

  const float dist = sqrtf(dist2);
  const vec3f wi = toLight * (1.f / dist);

  const float NoI = fmaxf(dot(prd.N, wi), 0.f);
  const float NoL = fmaxf(dot(lightSample.n, -wi), 0.f);
  if (NoI <= 0.f || NoL <= 0.f)
    return false;

  const vec3f f = bsdf.f(wo, wi);
  const float G = NoI * NoL / dist2;
  const vec3f unshadowedContribution = lightSample.Le * f * G;
  const float target = pt::restirTargetFromRgb(toPtVec(unshadowedContribution));
  if (target <= 0.f)
    return false;

  out.sample.lightId = lightSample.lightId;
  out.sample.uv = toPtVec(uSurface);
  out.sample.sourcePdf = lightSample.pdfA;
  out.sample.target = target;
  out.sample.position = toPtVec(lightSample.p);
  out.sample.normal = toPtVec(lightSample.n);
  out.sample.emission = toPtVec(lightSample.Le);
  out.wi = toPtVec(wi);
  out.unshadowedContribution = toPtVec(unshadowedContribution);
  return true;
}
