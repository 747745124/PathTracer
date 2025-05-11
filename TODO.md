# TODO

## CODE LEVEL
* ✅ Refactor code for dependency management
* ✅ Add std::variant to unify ConstantTexture and vec3 during construction

## TEST LEVEL
* ✅ Verify obj lodader correctness

## FEATURE LEVEL - Light
* ✅ Environment Light

## FEATURE LEVEL - Material
* ✅ Interface extension
* ✅ Rough dielectric
* ✅ Adding detailed dieletric interface
   * Per channel refract (i.e. dispersion)
* ✅ Disney Principled BSDF 2012 / 2015 Ver.
* Add Kajiya-Kay Material

## FEATURE LEVEL - Interface
* ✅ No longer need a separated lightlist explicitly, light discovery will be done in the scene
* Possibly medium / volume scattering

## FEATURE LEVEL - Rendering Method
* NEE+MIS+Power Heuristic
* BDPT

## FEATURE LEVEL - Sampler
* ✅ Halton Sampler
* ✅ Stratified Sampling
* ✅ Scrambled Halton Sampler
* Sobol sampler