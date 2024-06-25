#ifndef GCTK_MATH_H
#define GCTK_MATH_H

#include "gctk/common.h"

#include <math.h>
#include <cglm/cglm.h>
#include <cglm/quat.h>

#define GCTK_PI 3.141592653589793
#define GCTK_DEG_TO_RAD_VALUE 0.017453292519943295
#define GCTK_RAD_TO_DEG_VALUE 57.29577951308232

#define GctkDegToRad(__deg__) ((__deg__) * GCTK_DEG_TO_RAD_VALUE)
#define GctkRadToDeg(__rad__) ((__rad__) * GCTK_RAD_TO_DEG_VALUE)

#define GctkMax(__x__, __max__) ((__x__) > (__max__) ? (__max__) : (__x__))
#define GctkMin(__x__, __min__) ((__x__) < (__min__) ? (__min__) : (__x__))
#define GctkClamp(__x__, __min__, __max__) ((__x__) > (__max__) ? (__max__) : ((__x__) < (__min__) ? (__min__) : (__x__)))

typedef union Vec2i {
	struct { int x, y; };
	struct { int width, height; };
	int items[2];
} Vec2i;

typedef union Vec4i {
	struct { int x, y, z, w; };
	int items[4];
} Vec4i;

typedef union Vec2u {
	struct { unsigned x, y; };
	struct { unsigned width, height; };
	unsigned items[2];
} Vec2u;

typedef union Vec4u {
	struct { unsigned x, y, z, w; };
	unsigned items[4];
} Vec4u;

typedef union Vec2 {
	struct { float x, y; };
	vec2 items;
} Vec2;

typedef union Vec3 {
	struct { float x, y, z; };
	vec3 items;
} Vec3;

typedef union Vec4 {
	struct { float x, y, z, w; };
	vec4 items;
} Vec4;
typedef Vec4 Quat;

typedef union Vec2d {
	struct { double x, y; };
	double items[2];
} Vec2d;

typedef union Vec3d {
	struct { double x, y, z; };
	double items[3];
} Vec3d;

typedef union Vec4d {
	struct { double x, y, z, w; };
	double items[4];
} Vec4d;
typedef Vec4d QuatD;

typedef union Color {
	struct { float r, g, b, a; };
	float items[4];
} Color;

#define COLOR_WHITE ((Color){ 1.0, 1.0, 1.0, 1.0 })
#define COLOR_BLACK ((Color){ 0.0, 0.0, 0.0, 1.0 })

typedef union Color32 {
	struct { uint8_t r, g, b, a; };
	uint8_t items[4];
	uint32_t rgba;
} Color32;

typedef union Mat4 {
	struct {
		float i00, i01, i02, i03;
		float i10, i11, i12, i13;
		float i20, i21, i22, i23;
		float i30, i31, i32, i33;
	};
	float items[16];
	Vec4 columns[4];
	mat4 cglm_value;
} Mat4;

typedef struct AxisAngle {
	Vec3 axis;
	float angle;
} AxisAngle;

#define VEC2(__x, __y) ((Vec2){ __x, __y })
#define VEC2I(__x, __y) ((Vec2i){ __x, __y })
#define VEC2U(__x, __y) ((Vec2u){ __x, __y })
#define VEC3(__x, __y, __z) ((Vec3){ __x, __y, __z })
#define VEC4(__x, __y, __z, __w) ((Vec4){ __x, __y, __z, __w })
#define QUAT(__x, __y, __z, __w) ((Quat){ __x, __y, __z, __w })
#define MAT4(...) ((Mat4){ __VA_ARGS__ })
#define MAT4_COLUMNS(__col0__, __col1__, __col2__, __col3__) ((Mat4){ .columns = { (__col0__), (__col1__), (__col2__), (__col3__) } })
#define AXIS_ANGLE(__axis__, __angle__) ((AxisAngle){ Vec3Norm(__axis__), __angle__ })
#define COLOR(__r, __g, __b, __a) ((Color){ __r, __g, __b, __a })
#define COLOR_NO_ALPHA(__r, __g, __b) ((Color){ __r, __g, __b, 1.0f })
#define COLOR32(__r, __g, __b, __a) ((Color32){ __r, __g, __b, __a })

GCTK_API Vec2 Vec2Add(Vec2 lhs, Vec2 rhs);
GCTK_API Vec2 Vec2Sub(Vec2 lhs, Vec2 rhs);
GCTK_API Vec2 Vec2Mul(Vec2 lhs, Vec2 rhs);
GCTK_API Vec2 Vec2Div(Vec2 lhs, Vec2 rhs);
GCTK_API Vec2 Vec2MulF(Vec2 lhs, float rhs);
GCTK_API Vec2 Vec2DivF(Vec2 lhs, float rhs);
GCTK_API Vec2 Vec2Norm(Vec2 vec);
GCTK_API Vec2 Vec2Neg(Vec2 vec);
GCTK_API bool Vec2Equal(Vec2 lhs, Vec2 rhs);
GCTK_API float Vec2Dot(Vec2 lhs, Vec2 rhs);
GCTK_API float Vec2Dist(Vec2 lhs, Vec2 rhs);
GCTK_API float Vec2LenSqr(Vec2 vec);
GCTK_API float Vec2Len(Vec2 vec);

GCTK_API Vec3 Vec3Add(Vec3 lhs, Vec3 rhs);
GCTK_API Vec3 Vec3Sub(Vec3 lhs, Vec3 rhs);
GCTK_API Vec3 Vec3Mul(Vec3 lhs, Vec3 rhs);
GCTK_API Vec3 Vec3Div(Vec3 lhs, Vec3 rhs);
GCTK_API Vec3 Vec3MulF(Vec3 lhs, float rhs);
GCTK_API Vec3 Vec3DivF(Vec3 lhs, float rhs);
GCTK_API Vec3 Vec3Norm(Vec3 vec);
GCTK_API Vec3 Vec3Neg(Vec3 vec);
GCTK_API bool Vec3Equal(Vec3 lhs, Vec3 rhs);
GCTK_API float Vec3Dot(Vec3 lhs, Vec3 rhs);
GCTK_API Vec3 Vec3Cross(Vec3 lhs, Vec3 rhs);
GCTK_API float Vec3Dist(Vec3 lhs, Vec3 rhs);
GCTK_API float Vec3LenSqr(Vec3 vec);
GCTK_API float Vec3Len(Vec3 vec);
GCTK_API Vec3 Vec3FromVec2(Vec2 xy, float z);

GCTK_API Vec4 Vec4Add(Vec4 lhs, Vec4 rhs);
GCTK_API Vec4 Vec4Sub(Vec4 lhs, Vec4 rhs);
GCTK_API Vec4 Vec4Mul(Vec4 lhs, Vec4 rhs);
GCTK_API Vec4 Vec4Div(Vec4 lhs, Vec4 rhs);
GCTK_API Vec4 Vec4MulF(Vec4 lhs, float rhs);
GCTK_API Vec4 Vec4DivF(Vec4 lhs, float rhs);
GCTK_API Vec4 Vec4Norm(Vec4 vec);
GCTK_API Vec4 Vec4Neg(Vec4 vec);
GCTK_API bool Vec4Equal(Vec4 lhs, Vec4 rhs);
GCTK_API float Vec4Dot(Vec4 lhs, Vec4 rhs);
GCTK_API float Vec4Dist(Vec4 lhs, Vec4 rhs);
GCTK_API float Vec4LenSqr(Vec4 vec);
GCTK_API float Vec4Len(Vec4 vec);

// Aliases for Quaternion

#define QuatAdd Vec4Add
#define QuatSub Vec4Sub
#define QuatMul Vec4Mul
#define QuatDiv Vec4Div
#define QuatMulF Vec4MulF
#define QuatDivF Vec4DivF
#define QuatNorm Vec4Norm
#define QuatNeg Vec4Neg
#define QuatEqual Vec4Equal
#define QuatDot Vec4Dot
#define QuatLenSqr Vec4LenSqr
#define QuatLen Vec4Len

GCTK_API Quat QuatFromEuler(Vec3 euler);
GCTK_API Quat QuatFromEulerDeg(Vec3 euler);
GCTK_API Quat QuatFromAxisAngle(AxisAngle aa);
GCTK_API Quat QuatFromAxisAngleDeg(AxisAngle aa);
GCTK_API Vec3 QuatToEuler(Quat quat);
GCTK_API Vec3 QuatToEulerDeg(Quat quat);
GCTK_API AxisAngle QuatToAxisAngle(Quat quat);
GCTK_API AxisAngle QuatToAxisAngleDeg(Quat quat);
GCTK_API Vec3 EulerDegToRad(Vec3 euler);
GCTK_API Vec3 EulerRadToDeg(Vec3 euler);
GCTK_API AxisAngle EulerToAxisAngle(Vec3 euler);
GCTK_API AxisAngle EulerToAxisAngleDeg(Vec3 euler);
GCTK_API Vec3 EulerFromAxisAngle(AxisAngle aa);
GCTK_API Vec3 EulerFromAxisAngleDeg(AxisAngle aa);

#define Color32FromU32(__rgba__) ((Color32){ .rgba = (__rgba__) })

GCTK_API Color ColorFromColor32(Color32 color);
GCTK_API Color32 ColorToColor32(Color color);
GCTK_API bool ColorEqual(Color lhs, Color rhs);
GCTK_API bool ColorEqualIgnoreAlpha(Color lhs, Color rhs);
GCTK_API bool Color32Equal(Color32 lhs, Color32 rhs);
GCTK_API bool Color32EqualIgnoreAlpha(Color32 lhs, Color32 rhs);

GCTK_API Mat4 Mat4Mul(Mat4 a, Mat4 b);
GCTK_API Mat4 Mat4Transpose(Mat4 mat);
GCTK_API Mat4 Mat4Translate(Mat4 mat, Vec3 translation);
GCTK_API Mat4 Mat4Scale(Mat4 mat, Vec3 scale);
GCTK_API Mat4 Mat4Rotate(Mat4 mat, AxisAngle aa);
GCTK_API Mat4 Mat4RotateDeg(Mat4 mat, AxisAngle aa);
GCTK_API Mat4 Mat4RotateEuler(Mat4 mat, Vec3 euler);
GCTK_API Mat4 Mat4RotateEulerDeg(Mat4 mat, Vec3 euler);
GCTK_API Mat4 Mat4RotateQuat(Mat4 mat, Quat quat);
GCTK_API Mat4 Mat4Rotate2D(Mat4 mat, float angle);
GCTK_API Mat4 Mat4Rotate2DDeg(Mat4 mat, float angle);
GCTK_API Mat4 Mat4CreateOrtho(float left, float right, float bottom, float top, float nearZ, float farZ);
GCTK_API Mat4 Mat4CreatePerspective(float fov, float width, float height, float nearZ, float farZ);

GCTK_API Vec3      Mat4ExtractTranslation(Mat4 mat);
GCTK_API Vec3      Mat4ExtractScale(Mat4 mat);
GCTK_API AxisAngle Mat4ExtractRotation(Mat4 mat);
GCTK_API AxisAngle Mat4ExtractRotationDeg(Mat4 mat);
GCTK_API Vec3      Mat4ExtractRotationEuler(Mat4 mat);
GCTK_API Vec3      Mat4ExtractRotationEulerDeg(Mat4 mat);
GCTK_API Quat      Mat4ExtractRotationQuat(Mat4 mat);

#define VEC2_ZERO   VEC2(0.0f, 0.0f)
#define VEC2_UNIT_X VEC2(1.0f, 0.0f)
#define VEC2_UNIT_Y VEC2(0.0f, 1.0f)
#define VEC2_ONE    VEC2(1.0f, 1.0f)

#define VEC3_ZERO   VEC3(0.0f, 0.0f, 0.0f)
#define VEC3_UNIT_X VEC3(1.0f, 0.0f, 0.0f)
#define VEC3_UNIT_Y VEC3(0.0f, 1.0f, 0.0f)
#define VEC3_UNIT_Z VEC3(0.0f, 0.0f, 1.0f)
#define VEC3_ONE    VEC3(1.0f, 1.0f, 1.0f)

#define VEC4_ZERO   VEC4(0.0f, 0.0f, 0.0f, 0.0f)
#define VEC4_UNIT_X VEC4(1.0f, 0.0f, 0.0f, 0.0f)
#define VEC4_UNIT_Y VEC4(0.0f, 1.0f, 0.0f, 0.0f)
#define VEC4_UNIT_Z VEC4(0.0f, 0.0f, 1.0f, 0.0f)
#define VEC4_UNIT_W VEC4(0.0f, 0.0f, 0.0f, 1.0f)
#define VEC4_ONE    VEC4(1.0f, 1.0f, 1.0f, 1.0f)

#define AXIS_ANGLE_ZERO ((AxisAngle){ 0 })

#define QUAT_IDENTITY QUAT(0.0f, 0.0f, 0.0f, 1.0f)

#define MAT4_IDENTITY MAT4_COLUMNS(VEC4_UNIT_X, VEC4_UNIT_Y, VEC4_UNIT_Z, VEC4_UNIT_W)

typedef struct Transform2D Transform2D;

struct Transform2D {
	Mat4 matrix;
	Vec2 position, origin, scale;
	float angle, depth;
	Transform2D* parent;
};

typedef struct Transform3D Transform3D;
struct Transform3D {
	Mat4 matrix;
	Vec3 position, origin, scale;
	AxisAngle rotation;
	Transform3D* parent;
};

GCTK_API Transform2D CreateTransform2D(Vec2 position, Vec2 origin, Vec2 scale, float angle);
GCTK_API Transform2D CreateTransform2DDeg(Vec2 position, Vec2 origin, Vec2 scale, float angle);
GCTK_API Transform3D CreateTransform3D(Vec3 position, Vec3 origin, Vec3 scale, AxisAngle rotation);
GCTK_API Transform3D CreateTransform3DDeg(Vec3 position, Vec3 origin, Vec3 scale, AxisAngle rotation);

GCTK_API Vec2 Transform2DGetGlobalPosition(const Transform2D* transform);
GCTK_API float Transform2DGetGlobalDepth(const Transform2D* transform);
GCTK_API Vec2 Transform2DGetGlobalOrigin(const Transform2D* transform);
GCTK_API Vec2 Transform2DGetGlobalScale(const Transform2D* transform);
GCTK_API float Transform2DGetGlobalAngle(const Transform2D* transform);
GCTK_API float Transform2DGetGlobalAngleDeg(const Transform2D* transform);

GCTK_API void Transform2DSetGlobalPosition(Transform2D* transform, Vec2 pos);
GCTK_API void Transform2DSetGlobalDepth(Transform2D* transform, float depth);
GCTK_API void Transform2DSetGlobalOrigin(Transform2D* transform, Vec2 origin);
GCTK_API void Transform2DSetGlobalScale(Transform2D* transform, Vec2 scale);
GCTK_API void Transform2DSetGlobalAngle(Transform2D* transform, float angle);
GCTK_API void Transform2DSetGlobalAngleDeg(Transform2D* transform, float angle);

GCTK_API Vec3 Transform3DGetGlobalPosition(const Transform3D* transform);
GCTK_API Vec3 Transform3DGetGlobalOrigin(const Transform3D* transform);
GCTK_API Vec3 Transform3DGetGlobalScale(const Transform3D* transform);
GCTK_API AxisAngle Transform3DGetGlobalRotation(const Transform3D* transform);
GCTK_API AxisAngle Transform3DGetGlobalRotationDeg(const Transform3D* transform);
GCTK_API Vec3 Transform3DGetGlobalRotationEuler(const Transform3D* transform);
GCTK_API Vec3 Transform3DGetGlobalRotationEulerDeg(const Transform3D* transform);
GCTK_API Quat Transform3DGetGlobalRotationQuat(const Transform3D* transform);

GCTK_API void Transform3DSetGlobalPosition(Transform3D* transform, Vec3 pos);
GCTK_API void Transform3DSetGlobalOrigin(Transform3D* transform, Vec3 origin);
GCTK_API void Transform3DSetGlobalScale(Transform3D* transform, Vec3 scale);
GCTK_API void Transform3DSetGlobalRotation(Transform3D* transform, AxisAngle aa);
GCTK_API void Transform3DSetGlobalRotationDeg(Transform3D* transform, AxisAngle aa);
GCTK_API void Transform3DSetGlobalRotationEuler(Transform3D* transform, Vec3 euler);
GCTK_API void Transform3DSetGlobalRotationEulerDeg(Transform3D* transform, Vec3 euler);
GCTK_API void Transform3DSetGlobalRotationQuat(Transform3D* transform, Quat q);

GCTK_API Mat4 Transform2DUpdateMatrix(Transform2D* transform);
GCTK_API Mat4 Transform3DUpdateMatrix(Transform3D* transform);
GCTK_API Mat4 Transform2DGetMatrix(const Transform2D* transform);
GCTK_API Mat4 Transform3DGetMatrix(const Transform3D* transform);

typedef struct Viewport2D {
	Mat4 matrix;
	float left, right, top, bottom, nearZ, farZ;
} Viewport2D;

GCTK_API Viewport2D CreateViewport2D(float left, float right, float top, float bottom, float nearZ, float farZ);
GCTK_API Mat4 Viewport2DUpdateMatrix(Viewport2D* viewport);
GCTK_API Mat4 Viewport2DGetMatrix(const Viewport2D* viewport);

typedef struct Viewport3D {
	Mat4 matrix;
	float fov, width, height, nearZ, farZ;
} Viewport3D;

GCTK_API Viewport3D CreateViewport3D(float fov, float width, float height, float nearZ, float farZ);
GCTK_API Mat4 Viewport3DUpdateMatrix(Viewport3D* viewport);
GCTK_API Mat4 Viewport3DGetMatrix(const Viewport3D* viewport);

#endif