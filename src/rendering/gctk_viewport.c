#include "gctk/rendering/viewport.h"
#include "gctk/debug.h"

static Viewport2D GCTK_VIEWPORT_2D;
static Vec2       GCTK_VIEWPORT_2D_OFFSET;
static Viewport3D GCTK_VIEWPORT_3D;
static Vec3       GCTK_VIEWPORT_3D_OFFSET;
static AxisAngle  GCTK_VIEWPORT_3D_ROTATION;
static bool       GCTK_VIEWPORT_IS_2D;
static float      GCTK_VIEWPORT_SCALE = 1.0f;

bool GctkSetupViewport2D(int width, int height, Vec2 position, float nearClipping, float farClipping) {
	if (width <= 0 || height <= 0) {
		GctkLogError(GCTK_ERROR_OUT_OF_RANGE,
					 "Failed to setup viewport, window size is out of range: Width and height must be more then 0!");
		return false;
	}

	GCTK_VIEWPORT_2D        = CreateViewport2D(0, (float)width, (float)height, 0, nearClipping, farClipping);
	GCTK_VIEWPORT_2D_OFFSET = position;
	GCTK_VIEWPORT_IS_2D     = true;
	Viewport2DUpdateMatrix(&GCTK_VIEWPORT_2D);
	return true;
}

bool GctkSetupViewport3D(int width, int height, float fov, Vec3 position, AxisAngle rotation, float nearClipping,
						 float farClipping) {
	if (width <= 0 || height <= 0) {
		GctkLogError(GCTK_ERROR_OUT_OF_RANGE,
					 "Failed to setup viewport, window size is out of range: Width and height must be more then 0!");
		return false;
	}

	GCTK_VIEWPORT_3D          = CreateViewport3D(fov, (float)width, (float)height, nearClipping, farClipping);
	GCTK_VIEWPORT_3D_OFFSET   = position;
	GCTK_VIEWPORT_3D_ROTATION = rotation;
	GCTK_VIEWPORT_IS_2D       = false;
	return true;
}

void GctkUpdateViewport(int width, int height) {
	if (GCTK_VIEWPORT_IS_2D) {
		GCTK_VIEWPORT_2D.right = (float)width / GCTK_VIEWPORT_SCALE;
		GCTK_VIEWPORT_2D.top   = (float)height / GCTK_VIEWPORT_SCALE;
		Viewport2DUpdateMatrix(&GCTK_VIEWPORT_2D);
	} else {
		GCTK_VIEWPORT_3D.width = (float)width / GCTK_VIEWPORT_SCALE;
		GCTK_VIEWPORT_3D.height = (float)height / GCTK_VIEWPORT_SCALE;
		Viewport3DUpdateMatrix(&GCTK_VIEWPORT_3D);
	}
}
void GctkUpdateViewportUpdateCurrent() {
	if (GCTK_VIEWPORT_IS_2D) {
		Viewport2DUpdateMatrix(&GCTK_VIEWPORT_2D);
	} else {
		Viewport3DUpdateMatrix(&GCTK_VIEWPORT_3D);
	}
}

void GctkSetViewport2DPosition(Vec2 position) {
	GCTK_VIEWPORT_2D_OFFSET = position;
}
void GctkSetViewport3DPosition(Vec3 position) {
	GCTK_VIEWPORT_3D_OFFSET = position;
}
Vec2 GctkGetViewport2DPosition() {
	return GCTK_VIEWPORT_2D_OFFSET;
}
Vec3 GctkGetViewport3DPosition() {
	return GCTK_VIEWPORT_3D_OFFSET;
}

void GctkSetViewport3DRotation(AxisAngle rotation) {
	GCTK_VIEWPORT_3D_ROTATION = rotation;
}
AxisAngle GctkGetViewport3DRotation() {
	return GCTK_VIEWPORT_3D_ROTATION;
}

void GctkSetViewport3DFov(float fov) {
	GCTK_VIEWPORT_3D.fov = fov;
	Viewport3DUpdateMatrix(&GCTK_VIEWPORT_3D);
}

void GctkSetViewport2DNearClipping(float nearClipping) {
	GCTK_VIEWPORT_2D.nearZ = nearClipping;
	Viewport2DUpdateMatrix(&GCTK_VIEWPORT_2D);
}
void GctkSetViewport3DNearClipping(float nearClipping) {
	GCTK_VIEWPORT_3D.nearZ = nearClipping;
	Viewport3DUpdateMatrix(&GCTK_VIEWPORT_3D);
}

void GctkSetViewport2DFarClipping(float farClipping) {
	GCTK_VIEWPORT_2D.farZ = farClipping;
	Viewport2DUpdateMatrix(&GCTK_VIEWPORT_2D);
}
void GctkSetViewport3DFarClipping(float farClipping) {
	GCTK_VIEWPORT_3D.farZ = farClipping;
	Viewport3DUpdateMatrix(&GCTK_VIEWPORT_3D);
}

bool GctkIsViewport2D() {
	return GCTK_VIEWPORT_IS_2D;
}

Mat4 GctkGetViewportMatrix() {
	return GCTK_VIEWPORT_IS_2D ? GCTK_VIEWPORT_2D.matrix : GCTK_VIEWPORT_3D.matrix;
}

void GctkSetViewportScale(float scale) {
	if (scale > 0) GCTK_VIEWPORT_SCALE = scale;
}