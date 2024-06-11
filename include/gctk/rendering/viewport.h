#ifndef GCTK_VIEWPORT_H
#define GCTK_VIEWPORT_H

#include "gctk/common.h"
#include "gctk/math.h"

GCTK_API bool GctkSetupViewport2D(int width, int height, Vec2 position, float nearClipping, float farClipping);
GCTK_API bool GctkSetupViewport3D(int width, int height, float fov, Vec3 position, AxisAngle rotation,
								  float nearClipping, float farClipping);

GCTK_API void GctkUpdateViewport(int width, int height);
GCTK_API void GctkUpdateViewportUpdateCurrent();

GCTK_API void GctkSetViewport2DPosition(Vec2 position);
GCTK_API void GctkSetViewport3DPosition(Vec3 position);
GCTK_API Vec2 GctkGetViewport2DPosition();
GCTK_API Vec3 GctkGetViewport3DPosition();

GCTK_API void GctkSetViewport3DRotation(AxisAngle rotation);
GCTK_API AxisAngle GctkGetViewport3DRotation();

GCTK_API void GctkSetViewport3DFov(float fov);

GCTK_API void GctkSetViewport2DNearClipping(float nearClipping);
GCTK_API void GctkSetViewport3DNearClipping(float nearClipping);
GCTK_API void GctkSetViewport2DFarClipping(float farClipping);
GCTK_API void GctkSetViewport3DFarClipping(float farClipping);

GCTK_API bool GctkIsViewport2D();
GCTK_API Mat4 GctkGetViewportMatrix();

GCTK_API void GctkSetViewportScale(float scale);

#endif