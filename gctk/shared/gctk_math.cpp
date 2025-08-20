#include "gctk_math.hpp"

namespace gctk {
	const Vector2 Vector2::ZERO   = { 0.0f, 0.0f };
	const Vector2 Vector2::UNIT_X = { 1.0f, 0.0f };
	const Vector2 Vector2::UNIT_Y = { 0.0f, 1.0f };
	const Vector2 Vector2::ONE    = { 1.0f, 1.0f };

	const Point Point::ZERO   = { 0, 0 };
	const Point Point::UNIT_X = { 1, 0 };
	const Point Point::UNIT_Y = { 0, 1 };
	const Point Point::ONE    = { 1, 1 };

	const Vector3 Vector3::ZERO   = { 0.0f, 0.0f, 0.0f };
	const Vector3 Vector3::UNIT_X = { 1.0f, 0.0f, 0.0f };
	const Vector3 Vector3::UNIT_Y = { 0.0f, 1.0f, 0.0f };
	const Vector3 Vector3::UNIT_Z = { 0.0f, 0.0f, 1.0f };
	const Vector3 Vector3::ONE    = { 1.0f, 1.0f, 1.0f };

	const Vector4 Vector4::ZERO   = { 0.0f, 0.0f, 0.0f, 0.0f };
	const Vector4 Vector4::UNIT_X = { 1.0f, 0.0f, 0.0f, 0.0f };
	const Vector4 Vector4::UNIT_Y = { 0.0f, 1.0f, 0.0f, 0.0f };
	const Vector4 Vector4::UNIT_Z = { 0.0f, 0.0f, 1.0f, 0.0f };
	const Vector4 Vector4::UNIT_W = { 0.0f, 0.0f, 0.0f, 1.0f };
	const Vector4 Vector4::ONE    = { 1.0f, 1.0f, 1.0f, 1.0f };

	const Quaternion Quaternion::IDENTITY = { };

	const Matrix4 Matrix4::IDENTITY = { };
}
