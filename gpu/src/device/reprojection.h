// ======================================================================== //
// reprojection.h - device-side same-scene temporal reprojection helpers.
// ======================================================================== //

#pragma once

#include "device/prd.h"
#include "pod/launchParams.h"

using namespace owl;

__device__ inline bool reprojectCurrentHitToPreviousPixel(const LaunchParams &params,
                                                          const PRD &prd,
                                                          int &prevPxIdx)
{
  prevPxIdx = -1;
  if (!params.hasPreviousCamera || !prd.didHit) return false;

  const CameraFrameGPU &prevCamera = params.previousCamera;
  const vec3f du = prevCamera.dir_du;
  const vec3f dv = prevCamera.dir_dv;
  const float duLen2 = dot(du, du);
  const float dvLen2 = dot(dv, dv);
  if (duLen2 <= 1e-12f || dvLen2 <= 1e-12f) return false;

  const vec3f uAxis = du * rsqrtf(duLen2);
  const vec3f vAxis = dv * rsqrtf(dvLen2);
  const vec3f forward = normalize(cross(vAxis, uAxis));

  const vec3f toHit = prd.hitP - prevCamera.pos;
  const float planeScale = dot(toHit, forward);
  if (planeScale <= 1e-6f) return false;

  const vec3f previousRayPlane = toHit * (1.f / planeScale);
  const vec3f offset = previousRayPlane - prevCamera.dir_00;
  const float screenX = dot(offset, du) / duLen2;
  const float screenY = dot(offset, dv) / dvLen2;

  if (screenX < 0.f || screenX >= 1.f || screenY < 0.f || screenY >= 1.f) {
    return false;
  }

  const int x = min(int(screenX * float(params.fbSize.x)), params.fbSize.x - 1);
  const int y = min(int(screenY * float(params.fbSize.y)), params.fbSize.y - 1);
  prevPxIdx = x + y * params.fbSize.x;
  return true;
}

__device__ inline bool validateReprojectedSurface(const LaunchParams &params,
                                                  const PRD &prd,
                                                  int prevPxIdx)
{
  if (!params.prevRestirSurfaceData || prevPxIdx < 0) return false;

  const pt::RestirSurfaceData prevSurface = params.prevRestirSurfaceData[prevPxIdx];
  if (!prevSurface.valid || !prd.didHit) return false;
  if (prevSurface.materialId != prd.materialId) return false;

  const float normalAgreement = dot(prevSurface.normal, prd.N);
  if (normalAgreement < 0.9f) return false;

  const vec3f surfaceDelta = prevSurface.hitP - prd.hitP;
  const float surfaceDist2 = dot(surfaceDelta, surfaceDelta);
  if (surfaceDist2 > 0.01f) return false;

  return true;
}
