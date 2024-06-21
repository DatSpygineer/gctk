#include "gctk/math.h"

Vec2 Vec2Add(Vec2 lhs, Vec2 rhs) {
	return VEC2( lhs.x + rhs.x, lhs.y + rhs.y );
}
Vec2 Vec2Sub(Vec2 lhs, Vec2 rhs) {
	return VEC2( lhs.x - rhs.x, lhs.y - rhs.y );
}
Vec2 Vec2Mul(Vec2 lhs, Vec2 rhs) {
	return VEC2( lhs.x * rhs.x, lhs.y * rhs.y );
}
Vec2 Vec2Div(Vec2 lhs, Vec2 rhs) {
	return VEC2( lhs.x / rhs.x, lhs.y / rhs.y );
}
Vec2 Vec2MulF(Vec2 lhs, float rhs) {
	return VEC2( lhs.x * rhs, lhs.y * rhs );
}
Vec2 Vec2DivF(Vec2 lhs, float rhs) {
	return VEC2( lhs.x / rhs, lhs.y / rhs );
}
Vec2 Vec2Norm(Vec2 vec) {
	return Vec2DivF(vec, Vec2Len(vec));
}
Vec2 Vec2Neg(Vec2 vec) {
	return VEC2( -vec.x, -vec.y );
}
bool Vec2Equal(Vec2 lhs, Vec2 rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
float Vec2Dot(Vec2 lhs, Vec2 rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y;
}
float Vec2Dist(Vec2 lhs, Vec2 rhs) {
	return Vec2Len(Vec2Sub(rhs, lhs));
}
float Vec2LenSqr(Vec2 vec) {
	return vec.x * vec.x + vec.y * vec.y;
}
float Vec2Len(Vec2 vec) {
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

Vec3 Vec3Add(Vec3 lhs, Vec3 rhs) {
	return VEC3( lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z );
}
Vec3 Vec3Sub(Vec3 lhs, Vec3 rhs) {
	return VEC3( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z );
}
Vec3 Vec3Mul(Vec3 lhs, Vec3 rhs) {
	return VEC3( lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z );
}
Vec3 Vec3Div(Vec3 lhs, Vec3 rhs) {
	return VEC3( lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z );
}
Vec3 Vec3MulF(Vec3 lhs, float rhs) {
	return VEC3( lhs.x * rhs, lhs.y * rhs, lhs.z * rhs );
}
Vec3 Vec3DivF(Vec3 lhs, float rhs) {
	return VEC3( lhs.x / rhs, lhs.y / rhs, lhs.z / rhs );
}
Vec3 Vec3Norm(Vec3 vec) {
	return Vec3DivF(vec, Vec3Len(vec));
}
Vec3 Vec3Neg(Vec3 vec) {
	return VEC3( -vec.x, -vec.y, -vec.z );
}
bool Vec3Equal(Vec3 lhs, Vec3 rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
float Vec3Dot(Vec3 lhs, Vec3 rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
Vec3 Vec3Cross(Vec3 lhs, Vec3 rhs) {
	Vec3 result;
	glm_cross(lhs.items, rhs.items, result.items);
	return result;
}
float Vec3Dist(Vec3 lhs, Vec3 rhs) {
	return Vec3Len(Vec3Sub(rhs, lhs));
}
float Vec3LenSqr(Vec3 vec) {
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}
float Vec3Len(Vec3 vec) {
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}
Vec3 Vec3FromVec2(Vec2 xy, float z) {
	return VEC3( xy.x, xy.y, z );
}

Vec4 Vec4Add(Vec4 lhs, Vec4 rhs) {
	return VEC4( lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w );
}
Vec4 Vec4Sub(Vec4 lhs, Vec4 rhs) {
	return VEC4( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w );
}
Vec4 Vec4Mul(Vec4 lhs, Vec4 rhs) {
	return VEC4( lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w );
}
Vec4 Vec4Div(Vec4 lhs, Vec4 rhs) {
	return VEC4( lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w );
}
Vec4 Vec4MulF(Vec4 lhs, float rhs) {
	return VEC4( lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs );
}
Vec4 Vec4DivF(Vec4 lhs, float rhs) {
	return VEC4( lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs );
}
Vec4 Vec4Norm(Vec4 vec) {
	return Vec4DivF(vec, Vec4Len(vec));
}
Vec4 Vec4Neg(Vec4 vec) {
	return VEC4( -vec.x, -vec.y, -vec.z, -vec.w );
}
bool Vec4Equal(Vec4 lhs, Vec4 rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}
float Vec4Dot(Vec4 lhs, Vec4 rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}
float Vec4Dist(Vec4 lhs, Vec4 rhs) {
	return Vec4Len(Vec4Sub(rhs, lhs));
}
float Vec4LenSqr(Vec4 vec) {
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
}
float Vec4Len(Vec4 vec) {
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

Quat QuatFromEuler(Vec3 euler) {
	Quat q;
	glm_euler_xyz_quat(euler.items, q.items);
	return q;
}
Quat QuatFromEulerDeg(Vec3 euler) {
	return QuatFromEuler((Vec3){
		GctkDegToRad(euler.x),
		GctkDegToRad(euler.y),
		GctkDegToRad(euler.z)
	});
}
Quat QuatFromAxisAngle(AxisAngle aa) {
	Quat q;
	Vec3 axis_norm = Vec3Norm(aa.axis);
	glm_quat(q.items, aa.angle, axis_norm.x, axis_norm.y, axis_norm.z);
	return q;
}
Quat QuatFromAxisAngleDeg(AxisAngle aa) {
	aa.angle *= (float)GCTK_DEG_TO_RAD_VALUE;
	return QuatFromAxisAngle(aa);
}
Vec3 QuatToEuler(Quat quat) {
	float xx = quat.x * quat.x;
	float yy = quat.y * quat.y;
	float zz = quat.z * quat.z;

	return VEC3(
		atan2f(2.0f * (quat.x * quat.y + quat.z * quat.w), 1.0f - 2.0f * (xx + yy)),
		asinf(2.0f * (quat.x*quat.w - quat.y*quat.z)),
		atan2f(2.0f * (quat.x*quat.z + quat.y*quat.w), 1.0f - 2.0f * (yy + zz))
	);
}
Vec3 QuatToEulerDeg(Quat quat) {
	return EulerRadToDeg(QuatToEuler(quat));
}
AxisAngle QuatToAxisAngle(Quat quat) {
	AxisAngle aa;
	glm_quat_axis(quat.items, aa.axis.items);
	aa.angle = glm_quat_angle(quat.items);
	return aa;
}
AxisAngle QuatToAxisAngleDeg(Quat quat) {
	AxisAngle aa = QuatToAxisAngle(quat);
	aa.angle *= (float)GCTK_RAD_TO_DEG_VALUE;
	return aa;
}
Vec3 EulerDegToRad(Vec3 euler) {
	return VEC3(
		GctkDegToRad(euler.x),
		GctkDegToRad(euler.y),
		GctkDegToRad(euler.z)
	);
}
Vec3 EulerRadToDeg(Vec3 euler) {
	return VEC3(
		GctkRadToDeg(euler.x),
		GctkRadToDeg(euler.y),
		GctkRadToDeg(euler.z)
	);
}
AxisAngle EulerToAxisAngle(Vec3 euler) {
	return QuatToAxisAngle(QuatFromEuler(euler));
}
AxisAngle EulerToAxisAngleDeg(Vec3 euler) {
	AxisAngle aa = EulerToAxisAngle(EulerDegToRad(euler));
	aa.angle *= (float)GCTK_RAD_TO_DEG_VALUE;
	return aa;
}
Vec3 EulerFromAxisAngle(AxisAngle aa) {
	return QuatToEuler(QuatFromAxisAngle(aa));
}
Vec3 EulerFromAxisAngleDeg(AxisAngle aa) {
	aa.angle *= (float)GCTK_RAD_TO_DEG_VALUE;
	return EulerFromAxisAngle(aa);
}

Color ColorFromColor32(Color32 color) {
	return COLOR(
		.r = (float)color.r / 255.0f,
		.g = (float)color.g / 255.0f,
		.b = (float)color.b / 255.0f,
		.a = (float)color.a / 255.0f
	);
}
Color32 ColorToColor32(Color color) {
	return COLOR32(
		.r = (uint8_t)roundf(color.r * 255),
		.g = (uint8_t)roundf(color.g * 255),
		.b = (uint8_t)roundf(color.b * 255),
		.a = (uint8_t)roundf(color.a * 255)
	);
}

bool ColorEqual(Color lhs, Color rhs) {
	return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}
bool ColorEqualIgnoreAlpha(Color lhs, Color rhs) {
	return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}
bool Color32Equal(Color32 lhs, Color32 rhs) {
	return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}
bool Color32EqualIgnoreAlpha(Color32 lhs, Color32 rhs) {
	return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

Mat4 Mat4Mul(Mat4 a, Mat4 b) {
	Mat4 result;
	glm_mat4_mul(a.cglm_value, b.cglm_value, result.cglm_value);
	return result;
}
Mat4 Mat4Transpose(Mat4 mat) {
	Mat4 result = mat;
	glm_mat4_transpose(result.cglm_value);
	return result;
}
Mat4 Mat4Translate(Mat4 mat, Vec3 translation) {
	Mat4 result = mat;
	glm_translate(result.cglm_value, translation.items);
	return result;
}
Mat4 Mat4Scale(Mat4 mat, Vec3 scale) {
	Mat4 result = mat;
	glm_scale(result.cglm_value, scale.items);
	return result;
}
Mat4 Mat4Rotate(Mat4 mat, AxisAngle aa) {
	Mat4 result = mat;
	glm_rotate(result.cglm_value, aa.angle, aa.axis.items);
	return result;
}
Mat4 Mat4RotateDeg(Mat4 mat, AxisAngle aa) {
	return Mat4Rotate(mat, AXIS_ANGLE(aa.axis, GctkRadToDeg(aa.angle)));
}
Mat4 Mat4RotateEuler(Mat4 mat, Vec3 euler) {
	Mat4 euler_mat, result;
	glm_euler(euler.items, euler_mat.cglm_value);
	glm_mul(mat.cglm_value, euler_mat.cglm_value, result.cglm_value);
	return result;
}
Mat4 Mat4RotateEulerDeg(Mat4 mat, Vec3 euler) {
	return Mat4RotateEuler(mat, (Vec3) {
		GctkDegToRad(euler.x),
		GctkDegToRad(euler.y),
		GctkDegToRad(euler.z)
	});
}
Mat4 Mat4RotateQuat(Mat4 mat, Quat quat) {
	Vec3 axis;
	float angle;
	glm_quat_axis(quat.items, axis.items);
	angle = glm_quat_angle(quat.items);
	return Mat4Rotate(mat, AXIS_ANGLE( axis, angle ));
}
Mat4 Mat4Rotate2D(Mat4 mat, float angle) {
	return Mat4Rotate(mat, AXIS_ANGLE( VEC3_UNIT_Z, angle ));
}
Mat4 Mat4CreateRotation2D_Deg(Mat4 mat, float angle) {
	return Mat4RotateDeg(mat, AXIS_ANGLE( VEC3_UNIT_Z, angle ));
}
Mat4 Mat4CreateOrtho(float left, float right, float bottom, float top, float nearZ, float farZ) {
	Mat4 mat = MAT4_IDENTITY;
	glm_ortho(left, right, bottom, top, nearZ, farZ, mat.cglm_value);
	return mat;
}
Mat4 Mat4CreatePerspective(float fov, float width, float height, float nearZ, float farZ) {
	Mat4 mat = MAT4_IDENTITY;
	glm_perspective(fov, width / height, nearZ, farZ, mat.cglm_value);
	return mat;
}

Vec3 Mat4ExtractTranslation(Mat4 mat) {
	Vec3 translation;
	glm_vec3(mat.cglm_value[3], translation.items);
	return translation;
}
Vec3 Mat4ExtractScale(Mat4 mat) {
	Vec3 scale;
	vec3 basisX, basisY, basisZ;
	glm_vec3_copy(mat.cglm_value[0], basisX);
	glm_vec3_copy(mat.cglm_value[1], basisY);
	glm_vec3_copy(mat.cglm_value[2], basisZ);
	glm_vec3_scale(basisX, 1.0f / scale.items[0], basisX);
	glm_vec3_scale(basisY, 1.0f / scale.items[1], basisY);
	glm_vec3_scale(basisZ, 1.0f / scale.items[2], basisZ);

	return scale;
}
AxisAngle Mat4ExtractRotation(Mat4 mat) {
	return QuatToAxisAngle(Mat4ExtractRotationQuat(mat));
}
AxisAngle Mat4ExtractRotationDeg(Mat4 mat) {
	return QuatToAxisAngleDeg(Mat4ExtractRotationQuat(mat));
}
Vec3 Mat4ExtractRotationEuler(Mat4 mat) {
	return QuatToEuler(Mat4ExtractRotationQuat(mat));
}
Vec3 Mat4ExtractRotationEulerDeg(Mat4 mat) {
	return QuatToEulerDeg(Mat4ExtractRotationQuat(mat));
}
Quat Mat4ExtractRotationQuat(Mat4 mat) {
	mat3 rotation;
	vec3 basisX, basisY, basisZ;
	glm_mat3_copy((vec3[3]){ {basisX[0], basisX[1], basisX[2]},
							 {basisY[0], basisY[1], basisY[2]},
							 {basisZ[0], basisZ[1], basisZ[2]} }, rotation);
	Quat q;
	glm_mat3_quat(rotation, q.items);
	return q;
}

Transform2D CreateTransform2D(Vec2 position, Vec2 origin, Vec2 scale, float angle) {
	Transform2D transform = {
		.position = position,
		.origin = origin,
		.scale = scale,
		.angle = angle
	};

	Transform2DUpdateMatrix(&transform);
	return transform;
}
Transform2D CreateTransform2DDeg(Vec2 position, Vec2 origin, Vec2 scale, float angle) {
	Transform2D transform = {
		.position = position,
		.origin = origin,
		.scale = scale,
		.angle = GctkDegToRad(angle)
	};

	Transform2DUpdateMatrix(&transform);
	return transform;
}
Transform3D CreateTransform3D(Vec3 position, Vec3 origin, Vec3 scale, AxisAngle rotation) {
	Transform3D transform = {
		.position = position,
		.origin = origin,
		.scale = scale,
		.rotation = rotation
	};

	Transform3DUpdateMatrix(&transform);
	return transform;
}
Transform3D CreateTransform3DDeg(Vec3 position, Vec3 origin, Vec3 scale, AxisAngle rotation) {
	Transform3D transform = {
		.position = position,
		.origin = origin,
		.scale = scale,
		.rotation = AXIS_ANGLE(rotation.axis, GctkDegToRad(rotation.angle))
	};

	Transform3DUpdateMatrix(&transform);
	return transform;
}

Vec2 Transform2DGetGlobalPosition(const Transform2D* transform) {
	if (transform == NULL) return VEC2_ZERO;

	Vec2 pos = transform->position;
	if (transform->parent != NULL) {
		pos = Vec2Add(pos, Transform2DGetGlobalPosition(transform->parent));
	}
	return pos;
}
float Transform2DGetGlobalDepth(const Transform2D* transform) {
	if (transform == NULL) return 0;

	float depth = transform->depth;
	if (transform->parent != NULL) {
		depth += Transform2DGetGlobalDepth(transform->parent);
	}
	return depth;
}
Vec2 Transform2DGetGlobalOrigin(const Transform2D* transform) {
	if (transform == NULL) return VEC2_ZERO;

	if (transform->parent != NULL) {
		return Transform2DGetGlobalOrigin(transform->parent);
	}
	return transform->origin;
}
Vec2 Transform2DGetGlobalScale(const Transform2D* transform) {
	if (transform == NULL) return VEC2_ZERO;

	Vec2 scale = transform->scale;
	if (transform->parent != NULL) {
		scale = Vec2Mul(scale, Transform2DGetGlobalScale(transform->parent));
	}
	return scale;
}
float Transform2DGetGlobalAngle(const Transform2D* transform) {
	if (transform == NULL) return 0;

	float angle = transform->angle;
	if (transform->parent != NULL) {
		angle += Transform2DGetGlobalAngle(transform->parent);
	}
	return angle;
}
float Transform2DGetGlobalAngleDeg(const Transform2D* transform) {
	return GctkRadToDeg(Transform2DGetGlobalAngle(transform));
}
void Transform2DSetGlobalPosition(Transform2D* transform, Vec2 pos) {
	if (transform == NULL) return;

	if (transform->parent != NULL) {
		transform->position = Vec2Sub(pos, Transform2DGetGlobalPosition(transform->parent));
	} else {
		transform->position = pos;
	}
}
void Transform2DSetGlobalDepth(Transform2D* transform, float depth) {
	if (transform == NULL) return;

	if (transform->parent != NULL) {
		transform->depth = depth - Transform2DGetGlobalDepth(transform->parent);
	} else {
		transform->depth = depth;
	}
}
void Transform2DSetGlobalOrigin(Transform2D* transform, Vec2 origin) {
	if (transform == NULL) return;
	transform->origin = origin;
}
void Transform2DSetGlobalScale(Transform2D* transform, Vec2 scale) {
	if (transform == NULL) return;

	if (transform->parent != NULL) {
		transform->scale = Vec2Div(scale, Transform2DGetGlobalScale(transform->parent));
	} else {
		transform->scale = scale;
	}
}
void Transform2DSetGlobalAngle(Transform2D* transform, float angle) {
	if (transform == NULL) return;

	if (transform->parent != NULL) {
		transform->angle = angle - Transform2DGetGlobalAngle(transform->parent);
	} else {
		transform->angle = angle;
	}
}
void Transform2DSetGlobalAngleDeg(Transform2D* transform, float angle) {
	Transform2DSetGlobalAngle(transform, GctkDegToRad(angle));
}

Vec3 Transform3DGetGlobalPosition(const Transform3D* transform) {
	if (transform == NULL) return VEC3_ZERO;

	Vec3 pos = transform->position;
	if (transform->parent != NULL) {
		pos = Vec3Add(pos, Transform3DGetGlobalPosition(transform->parent));
	}
	return pos;
}
Vec3 Transform3DGetGlobalOrigin(const Transform3D* transform) {
	if (transform == NULL) return VEC3_ZERO;

	if (transform->parent != NULL) {
		return Transform3DGetGlobalOrigin(transform->parent);
	}
	return transform->origin;
}
Vec3 Transform3DGetGlobalScale(const Transform3D* transform) {
	if (transform == NULL) return VEC3_ZERO;

	Vec3 scale = transform->position;
	if (transform->parent != NULL) {
		scale = Vec3Mul(scale, Transform3DGetGlobalScale(transform->parent));
	}
	return scale;
}
AxisAngle Transform3DGetGlobalRotation(const Transform3D* transform) {
	if (transform == NULL) return AXIS_ANGLE_ZERO;

	AxisAngle aa = transform->rotation;
	if (transform->parent != NULL) {
		AxisAngle aa2 = Transform3DGetGlobalRotation(transform->parent);
		aa.axis = Vec3Add(aa.axis, aa2.axis);
		aa.angle += aa2.angle;
	}
	return aa;
}
AxisAngle Transform3DGetGlobalRotationDeg(const Transform3D* transform) {
	if (transform == NULL) return AXIS_ANGLE_ZERO;

	AxisAngle aa = Transform3DGetGlobalRotation(transform);
	aa.angle *= (float)GCTK_RAD_TO_DEG_VALUE;
	return aa;
}
Vec3 Transform3DGetGlobalRotationEuler(const Transform3D* transform) {
	if (transform == NULL) return VEC3_ZERO;

	return EulerFromAxisAngle(Transform3DGetGlobalRotation(transform));
}
Vec3 Transform3DGetGlobalRotationEulerDeg(const Transform3D* transform) {
	if (transform == NULL) return VEC3_ZERO;

	return EulerRadToDeg(Transform3DGetGlobalRotationEuler(transform));
}
Quat Transform3DGetGlobalRotationQuat(const Transform3D* transform) {
	if (transform == NULL) return QUAT_IDENTITY;

	return QuatFromAxisAngle(Transform3DGetGlobalRotation(transform));
}

void Transform3DSetGlobalPosition(Transform3D* transform, Vec3 pos) {
	if (transform == NULL) return;

	if (transform->parent != NULL) {
		transform->position = Vec3Sub(pos, Transform3DGetGlobalPosition(transform->parent));
	} else {
		transform->position = pos;
	}
}
void Transform3DSetGlobalOrigin(Transform3D* transform, Vec3 origin) {
	if (transform == NULL) return;
	transform->origin = origin;
}
void Transform3DSetGlobalScale(Transform3D* transform, Vec3 scale) {
	if (transform == NULL) return;

	if (transform->parent != NULL) {
		transform->scale = Vec3Div(scale, Transform3DGetGlobalScale(transform->parent));
	} else {
		transform->scale = scale;
	}
}
void Transform3DSetGlobalRotation(Transform3D* transform, AxisAngle aa) {
	if (transform == NULL) return;

	transform->rotation = aa;
	if (transform->parent != NULL) {
		AxisAngle aa_parent = Transform3DGetGlobalRotation(transform);
		aa.axis = Vec3Norm(Vec3Sub(aa.axis, aa_parent.axis));
		aa.angle -= aa_parent.angle;
	}
}
void Transform3DSetGlobalRotationDeg(Transform3D* transform, AxisAngle aa) {
	if (transform == NULL) return;

	aa.angle *= (float)GCTK_DEG_TO_RAD_VALUE;
	Transform3DSetGlobalRotation(transform, aa);
}
void Transform3DSetGlobalRotationEuler(Transform3D* transform, Vec3 euler) {
	Transform3DSetGlobalRotation(transform, EulerToAxisAngle(euler));
}
void Transform3DSetGlobalRotationEulerDeg(Transform3D* transform, Vec3 euler) {
	Transform3DSetGlobalRotation(transform, EulerToAxisAngleDeg(euler));
}
void Transform3DSetGlobalRotationQuat(Transform3D* transform, Quat q) {
	Transform3DSetGlobalRotation(transform, QuatToAxisAngle(q));
}

Mat4 Transform2DUpdateMatrix(Transform2D* transform) {
	if (transform == NULL) return MAT4_IDENTITY;
	transform->matrix = Transform2DGetMatrix(transform);
	return transform->matrix;
}
Mat4 Transform3DUpdateMatrix(Transform3D* transform) {
	if (transform == NULL) return MAT4_IDENTITY;
	transform->matrix = Transform3DGetMatrix(transform);
	return transform->matrix;
}
Mat4 Transform2DGetMatrix(const Transform2D* transform) {
	if (transform == NULL) return MAT4_IDENTITY;

	Mat4 mat = MAT4_IDENTITY;
	mat = Mat4Translate(mat, Vec3FromVec2(Transform2DGetGlobalPosition(transform), Transform2DGetGlobalDepth(transform)));
	mat = Mat4Translate(mat, Vec3FromVec2(Transform2DGetGlobalOrigin(transform), 0.0f));
	mat = Mat4Rotate2D(mat, Transform2DGetGlobalAngle(transform));
	mat = Mat4Translate(mat, Vec3Neg(Vec3FromVec2(Transform2DGetGlobalOrigin(transform), 0.0f)));
	mat = Mat4Scale(mat, Vec3FromVec2(Transform2DGetGlobalScale(transform), 1.0f));

	return mat;
}
Mat4 Transform3DGetMatrix(const Transform3D* transform) {
	if (transform == NULL) return MAT4_IDENTITY;

	Mat4 mat = MAT4_IDENTITY;
	mat = Mat4Translate(mat, Transform3DGetGlobalPosition(transform));
	mat = Mat4Translate(mat, Transform3DGetGlobalOrigin(transform));
	mat = Mat4Rotate(mat, Transform3DGetGlobalRotation(transform));
	mat = Mat4Translate(mat, Vec3Neg(Transform3DGetGlobalOrigin(transform)));
	mat = Mat4Scale(mat, Transform3DGetGlobalScale(transform));
	return mat;
}

Viewport2D CreateViewport2D(float left, float right, float top, float bottom, float nearZ, float farZ) {
	Viewport2D viewport = {
		.left = left,
		.right = right,
		.top = top,
		.bottom = bottom,
		.nearZ = nearZ,
		.farZ = farZ
	};
	Viewport2DUpdateMatrix(&viewport);
	return viewport;
}
Mat4 Viewport2DUpdateMatrix(Viewport2D* viewport) {
	if (viewport == NULL) return MAT4_IDENTITY;
	return viewport->matrix = Viewport2DGetMatrix(viewport);
}
Mat4 Viewport2DGetMatrix(const Viewport2D* viewport) {
	if (viewport == NULL) return MAT4_IDENTITY;
	return Mat4CreateOrtho(viewport->left, viewport->right, viewport->bottom, viewport->top,
						   viewport->nearZ, viewport->farZ);
}

Viewport3D CreateViewport3D(float fov, float width, float height, float nearZ, float farZ) {
	Viewport3D viewport = {
		.fov = fov,
		.width = width,
		.height = height,
		.nearZ = nearZ,
		.farZ = farZ
	};
	Viewport3DUpdateMatrix(&viewport);
	return viewport;
}
Mat4 Viewport3DUpdateMatrix(Viewport3D* viewport) {
	if (viewport == NULL) return MAT4_IDENTITY;
	return viewport->matrix = Viewport3DGetMatrix(viewport);
}
Mat4 Viewport3DGetMatrix(const Viewport3D* viewport) {
	if (viewport == NULL) return MAT4_IDENTITY;
	return Mat4CreatePerspective(viewport->fov, viewport->width, viewport->height, viewport->nearZ,
								 viewport->farZ);
}