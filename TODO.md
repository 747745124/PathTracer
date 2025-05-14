# TODO

## CODE LEVEL
* ✅ Refactor code for dependency management
* ✅ Add std::variant to unify ConstantTexture and vec3 during construction

## TEST LEVEL
* ✅ Verify obj lodader correctness

## FEATURE LEVEL - Light
* ✅ Environment Light
   * Importance sampling to HDRI

## FEATURE LEVEL - Material
* ✅ Interface extension
* ✅ Rough dielectric
* ✅ Adding detailed dieletric interface
   * Per channel refract (i.e. dispersion)
* ✅ Disney Principled BSDF 2012 / 2015 Ver.
* Add Kajiya-Kay Material
* Layered BSDF model
   * Some BSSRDF Maybe

## FEATURE LEVEL - Medium
* ✅ One Global Homogeneous Medium with Absorption
* Homogeneous Medium with Single Scattering
* Homogeneous Medium with Multiple Scattering
* Heterogeneous Medium with BDPT

## FEATURE LEVEL - Interface
* ✅ No longer need a separated lightlist explicitly, light discovery will be done in the scene

## FEATURE LEVEL - Rendering Method
* NEE + MIS+Power Heuristic
* BDPT

## FEATURE LEVEL - Sampler
* ✅ Halton Sampler
* ✅ Stratified Sampling
* ✅ Scrambled Halton Sampler
* Sobol sampler