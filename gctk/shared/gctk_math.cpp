#include "gctk_math.hpp"

namespace gctk {
	const Vector2 Vector2::ZERO   = { 0.0f, 0.0f };
	const Vector2 Vector2::UNIT_X = { 1.0f, 0.0f };
	const Vector2 Vector2::UNIT_Y = { 0.0f, 1.0f };
	const Vector2 Vector2::ONE    = { 1.0f, 1.0f };

	Vector2 Vector2::Min(const Vector2& a, const Vector2& b) {
		return Vector2 { Math::Min(a.x, b.x), Math::Min(a.y, b.y) };
	}
	Vector2 Vector2::Max(const Vector2& a, const Vector2& b) {
		return Vector2 { Math::Max(a.x, b.x), Math::Max(a.y, b.y) };
	}
	Vector2 Vector2::Clamp(const Vector2& x, const Vector2& min, const Vector2& max) {
		return Vector2 { Math::Clamp(x.x, min.x, max.x), Math::Clamp(x.y, min.y, max.y) };
	}
	Vector2 Vector2::Lerp(const Vector2& x, const Vector2& y, const float t) {
		return Vector2 {
			Math::Lerp(x.x, y.x, t),
			Math::Lerp(x.y, y.y, t)
		};
	}

	const Point Point::ZERO   = { 0, 0 };
	const Point Point::UNIT_X = { 1, 0 };
	const Point Point::UNIT_Y = { 0, 1 };
	const Point Point::ONE    = { 1, 1 };

	Point Point::Min(const Point& a, const Point& b) {
		return Point { Math::Min(a.x, b.x), Math::Min(a.y, b.y) };
	}
	Point Point::Max(const Point& a, const Point& b) {
		return Point { Math::Max(a.x, b.x), Math::Max(a.y, b.y) };
	}
	Point Point::Clamp(const Point& x, const Point& min, const Point& max) {
		return Point { Math::Clamp(x.x, min.x, max.x), Math::Clamp(x.y, min.y, max.y) };
	}

	const Vector3 Vector3::ZERO   = { 0.0f, 0.0f, 0.0f };
	const Vector3 Vector3::UNIT_X = { 1.0f, 0.0f, 0.0f };
	const Vector3 Vector3::UNIT_Y = { 0.0f, 1.0f, 0.0f };
	const Vector3 Vector3::UNIT_Z = { 0.0f, 0.0f, 1.0f };
	const Vector3 Vector3::ONE    = { 1.0f, 1.0f, 1.0f };

	Vector3 Vector3::Min(const Vector3& a, const Vector3& b) {
		return Vector3 { Math::Min(a.x, b.x), Math::Min(a.y, b.y), Math::Min(a.z, b.z) };
	}
	Vector3 Vector3::Max(const Vector3& a, const Vector3& b) {
		return Vector3 { Math::Max(a.x, b.x), Math::Max(a.y, b.y), Math::Max(a.z, b.z) };
	}
	Vector3 Vector3::Clamp(const Vector3& x, const Vector3& min, const Vector3& max) {
		return Vector3 {
			Math::Clamp(x.x, min.x, max.x),
			Math::Clamp(x.y, min.y, max.y),
			Math::Clamp(x.z, min.z, max.z)
		};
	}
	Vector3 Vector3::Lerp(const Vector3& x, const Vector3& y, const float t) {
		return Vector3 {
			Math::Lerp(x.x, y.x, t),
			Math::Lerp(x.y, y.y, t),
			Math::Lerp(x.z, y.z, t)
		};
	}

	const Vector4 Vector4::ZERO   = { 0.0f, 0.0f, 0.0f, 0.0f };
	const Vector4 Vector4::UNIT_X = { 1.0f, 0.0f, 0.0f, 0.0f };
	const Vector4 Vector4::UNIT_Y = { 0.0f, 1.0f, 0.0f, 0.0f };
	const Vector4 Vector4::UNIT_Z = { 0.0f, 0.0f, 1.0f, 0.0f };
	const Vector4 Vector4::UNIT_W = { 0.0f, 0.0f, 0.0f, 1.0f };
	const Vector4 Vector4::ONE    = { 1.0f, 1.0f, 1.0f, 1.0f };

	Vector4 Vector4::Min(const Vector4& a, const Vector4& b) {
		return Vector4 {
			Math::Min(a.x, b.x),
			Math::Min(a.y, b.y),
			Math::Min(a.z, b.z),
			Math::Min(a.w, b.w)
		};
	}
	Vector4 Vector4::Max(const Vector4& a, const Vector4& b) {
		return Vector4 {
			Math::Max(a.x, b.x),
			Math::Max(a.y, b.y),
			Math::Max(a.z, b.z),
			Math::Max(a.w, b.w)
		};
	}
	Vector4 Vector4::Clamp(const Vector4& x, const Vector4& min, const Vector4& max) {
		return Vector4 {
			Math::Clamp(x.x, min.x, max.x),
			Math::Clamp(x.y, min.y, max.y),
			Math::Clamp(x.z, min.z, max.z),
			Math::Clamp(x.w, min.w, max.w)
		};
	}
	Vector4 Vector4::Lerp(const Vector4& x, const Vector4& y, const float t) {
		return Vector4 {
			Math::Lerp(x.x, y.x, t),
			Math::Lerp(x.y, y.y, t),
			Math::Lerp(x.z, y.z, t),
			Math::Lerp(x.w, y.w, t)
		};
	}

	const Quaternion Quaternion::IDENTITY = { };

	const Matrix4 Matrix4::IDENTITY = { };
}
