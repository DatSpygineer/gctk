#pragma once

#include <chrono>
#include <format>
#include <cmath>
#include <random>
#include <tuple>

namespace gctk {
	template<typename T>
	concept FloatType = std::is_floating_point_v<T>;
	template<typename T>
	concept IntegerType = std::is_integral_v<T>;
	template<typename T>
	concept NumericType = std::is_floating_point_v<T> || std::is_integral_v<T>;

	namespace Math {
		template<FloatType T>
		constexpr T Pi = M_PI;
		template<FloatType T>
		constexpr T HalfPi = M_PI_2;
		template<FloatType T>
		constexpr T TwoPi = M_2_PI;

		template<NumericType T>
		constexpr T Min(T x, T y) { return std::min(x, y); }
		template<NumericType T>
		constexpr T Max(T x, T y) { return std::max(x, y); }
		template<NumericType T>
		constexpr T Clamp(T x, T min, T max) { return std::max(min, std::min(max, x)); }

		template<NumericType T>
		constexpr T Abs(T x) { return std::abs(x); }
		template<NumericType T>
		constexpr T Sign(T x) { return x < 0 ? -1 : (x > 0 ? 1 : 0); }

		template<FloatType T>
		constexpr T Sin(T x) { return std::sin(x); }
		template<FloatType T>
		constexpr T Cos(T x) { return std::cos(x); }
		template<FloatType T>
		constexpr T Tan(T x) { return std::tan(x); }
		template<FloatType T>
		constexpr T ASin(T x) { return std::asin(x); }
		template<FloatType T>
		constexpr T ACos(T x) { return std::acos(x); }
		template<FloatType T>
		constexpr T ATan(T x) { return std::atan(x); }
		template<FloatType T>
		constexpr T ATan2(T y, T x) { return std::atan2(y, x); }

		template<FloatType T>
		constexpr T Sqrt(T x) { return std::sqrt(x); }

		template<FloatType T>
		constexpr T Lerp(T min, T max, T blend) {
			return min + (max - min) * blend;
		}
		template<FloatType T>
		constexpr T GetBlendAmount(T x, T min, T max) {
			return (x - min) / (max - min);
		}
		template<FloatType T>
		constexpr T Remap(T x, T min_from, T max_from, T min_to, T max_to) {
			return Lerp(min_to, max_to, GetBlendAmount(x, min_from, max_from));
		}

		template<FloatType T>
		constexpr T Deg2Rad(const T& x) {
			return x * 0.017453292519943295;
		}
		template<FloatType T>
		constexpr T Rad2Deg(const T& x) {
			return x * 57.29577951308232;
		}
	}

	struct Vector2 {
		float x, y;

		constexpr Vector2() : x(0.0f), y(0.0f) {}
		constexpr Vector2(const float x, const float y) : x(x), y(y) {}

		[[nodiscard]] constexpr float length() const { return std::sqrt(x * x + y * y); }
		[[nodiscard]] constexpr float length_square() const { return x * x + y * y; }
		[[nodiscard]] inline std::string to_string() const { return std::format("{}, {}", x, y); }
		[[nodiscard]] constexpr float dot(const Vector2& other) const { return x * other.x + y * other.y; }
		[[nodiscard]] constexpr float distance(const Vector2& other) const { return (other - *this).length(); }
		[[nodiscard]] constexpr Vector2 normalized() const { return *this / length(); }
		[[nodiscard]] constexpr std::tuple<float, float> items() const { return std::make_tuple(x, y); }

		static Vector2 Min(const Vector2& a, const Vector2& b);
		static Vector2 Max(const Vector2& a, const Vector2& b);
		static Vector2 Clamp(const Vector2& x, const Vector2& min, const Vector2& max);
		static Vector2 Lerp(const Vector2& x, const Vector2& y, float t);

		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
		static const Vector2 ONE;
		static const Vector2 ZERO;

		[[nodiscard]] constexpr Vector2 operator+ (const Vector2& rhs) const {
			return Vector2 { x + rhs.x, y + rhs.y };
		}
		[[nodiscard]] constexpr Vector2 operator- (const Vector2& rhs) const {
			return Vector2 { x - rhs.x, y - rhs.y };
		}
		[[nodiscard]] constexpr Vector2 operator* (const Vector2& rhs) const {
			return Vector2 { x * rhs.x, y * rhs.y };
		}
		[[nodiscard]] constexpr Vector2 operator* (const float rhs) const {
			return Vector2 { x * rhs, y * rhs };
		}
		[[nodiscard]] constexpr Vector2 operator/ (const Vector2& rhs) const {
			return Vector2 { x / rhs.x, y / rhs.y };
		}
		[[nodiscard]] constexpr Vector2 operator/ (const float rhs) const {
			return Vector2 { x / rhs, y / rhs };
		}
		[[nodiscard]] constexpr Vector2 operator- () const {
			return Vector2 { -x, -y };
		}
		[[nodiscard]] constexpr bool operator== (const Vector2& rhs) const {
			return x == rhs.x && y == rhs.y;
		}
		[[nodiscard]] constexpr bool operator!= (const Vector2& rhs) const {
			return x != rhs.x || y != rhs.y;
		}

		Vector2& operator+= (const Vector2& rhs) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}
		Vector2& operator-= (const Vector2& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}
		Vector2& operator*= (const Vector2& rhs) {
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}
		Vector2& operator*= (const float rhs) {
			x *= rhs;
			y *= rhs;
			return *this;
		}
		Vector2& operator/= (const Vector2& rhs) {
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}
		Vector2& operator/= (const float rhs) {
			x /= rhs;
			y /= rhs;
			return *this;
		}

		constexpr float operator[] (const int index) const {
			if (index == 0) return x;
			if (index == 1) return y;
			throw std::out_of_range("Index out of range");
		}
		float& operator[] (const int index) {
			if (index == 0) return x;
			if (index == 1) return y;
			throw std::out_of_range("Index out of range");
		}
	};

	struct Point {
		int32_t x, y;

		constexpr Point() : x(0), y(0) { }
		constexpr Point(const int32_t x, const int32_t y) : x(x), y(y) { }

		[[nodiscard]] constexpr float length() const { return std::sqrt(static_cast<float>(x * x + y * y)); }
		[[nodiscard]] constexpr int32_t length_square() const { return x * x + y * y; }
		[[nodiscard]] inline std::string to_string() const { return std::format("{}, {}", x, y); }
		[[nodiscard]] constexpr int32_t dot(const Point& other) const { return x * other.x + y * other.y; }
		[[nodiscard]] constexpr float distance(const Point& other) const { return (other - *this).length(); }
		[[nodiscard]] constexpr Point normalized() const { return *this / static_cast<int32_t>(length()); }
		[[nodiscard]] constexpr std::tuple<int32_t, int32_t> items() const { return std::make_tuple(x, y); }

		static Point Min(const Point& a, const Point& b);
		static Point Max(const Point& a, const Point& b);
		static Point Clamp(const Point& x, const Point& min, const Point& max);

		static const Point UNIT_X;
		static const Point UNIT_Y;
		static const Point ONE;
		static const Point ZERO;

		[[nodiscard]] constexpr Point operator+ (const Point& rhs) const {
			return Point { x + rhs.x, y + rhs.y };
		}
		[[nodiscard]] constexpr Point operator- (const Point& rhs) const {
			return Point { x - rhs.x, y - rhs.y };
		}
		[[nodiscard]] constexpr Point operator* (const Point& rhs) const {
			return Point { x * rhs.x, y * rhs.y };
		}
		[[nodiscard]] constexpr Point operator* (const int32_t rhs) const {
			return Point { x * rhs, y * rhs };
		}
		[[nodiscard]] constexpr Point operator/ (const Point& rhs) const {
			return Point { x / rhs.x, y / rhs.y };
		}
		[[nodiscard]] constexpr Point operator/ (const int32_t rhs) const {
			return Point { x / rhs, y / rhs };
		}
		[[nodiscard]] constexpr Point operator- () const {
			return Point { -x, -y };
		}
		[[nodiscard]] constexpr bool operator== (const Point& rhs) const {
			return x == rhs.x && y == rhs.y;
		}
		[[nodiscard]] constexpr bool operator!= (const Point& rhs) const {
			return x != rhs.x || y != rhs.y;
		}

		Point& operator+= (const Point& rhs) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}
		Point& operator-= (const Point& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}
		Point& operator*= (const Point& rhs) {
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}
		Point& operator*= (const int32_t rhs) {
			x *= rhs;
			y *= rhs;
			return *this;
		}
		Point& operator/= (const Point& rhs) {
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}
		Point& operator/= (const int32_t rhs) {
			x /= rhs;
			y /= rhs;
			return *this;
		}

		constexpr int32_t operator[] (const int index) const {
			if (index == 0) return x;
			if (index == 1) return y;
			throw std::out_of_range("Index out of range");
		}
		int32_t& operator[] (const int index) {
			if (index == 0) return x;
			if (index == 1) return y;
			throw std::out_of_range("Index out of range");
		}
	};
	using FPoint = Vector2;

	struct Size {
		int32_t width, height;

		constexpr Size() : width(0), height(0) { }
		constexpr Size(const int32_t width, const int32_t height) : width(width), height(height) { }

		[[nodiscard]] constexpr Size operator+ (const Size& rhs) const {
			return Size { width + rhs.width, height + rhs.height };
		}
		[[nodiscard]] constexpr Size operator- (const Size& rhs) const {
			return Size { width - rhs.width, height - rhs.height };
		}
		[[nodiscard]] constexpr Size operator* (const Size& rhs) const {
			return Size { width * rhs.width, height * rhs.height };
		}
		[[nodiscard]] constexpr Size operator* (const int32_t rhs) const {
			return Size { width * rhs, height * rhs };
		}
		[[nodiscard]] constexpr Size operator/ (const Size& rhs) const {
			return Size { width / rhs.width, height / rhs.height };
		}
		[[nodiscard]] constexpr Size operator/ (const int32_t rhs) const {
			return Size { width / rhs, height / rhs };
		}
		[[nodiscard]] constexpr Size operator- () const {
			return Size { -width, -height };
		}
		[[nodiscard]] constexpr bool operator== (const Size& rhs) const {
			return width == rhs.width && height == rhs.height;
		}
		[[nodiscard]] constexpr bool operator!= (const Size& rhs) const {
			return width != rhs.width || height != rhs.height;
		}

		Size& operator+= (const Size& rhs) {
			width += rhs.width;
			height += rhs.height;
			return *this;
		}
		Size& operator-= (const Size& rhs) {
			width -= rhs.width;
			height -= rhs.height;
			return *this;
		}
		Size& operator*= (const Size& rhs) {
			width *= rhs.width;
			height *= rhs.height;
			return *this;
		}
		Size& operator*= (const int32_t rhs) {
			width *= rhs;
			height *= rhs;
			return *this;
		}
		Size& operator/= (const Size& rhs) {
			width /= rhs.width;
			height /= rhs.height;
			return *this;
		}
		Size& operator/= (const int32_t rhs) {
			width /= rhs;
			height /= rhs;
			return *this;
		}
	};
	struct FSize {
		float width, height;

		constexpr FSize() : width(0), height(0) { }
		constexpr FSize(const float width, const float height) : width(width), height(height) { }

		[[nodiscard]] constexpr FSize operator+ (const FSize& rhs) const {
			return FSize { width + rhs.width, height + rhs.height };
		}
		[[nodiscard]] constexpr FSize operator- (const FSize& rhs) const {
			return FSize { width - rhs.width, height - rhs.height };
		}
		[[nodiscard]] constexpr FSize operator* (const FSize& rhs) const {
			return FSize { width * rhs.width, height * rhs.height };
		}
		[[nodiscard]] constexpr FSize operator* (const int32_t rhs) const {
			return FSize { width * rhs, height * rhs };
		}
		[[nodiscard]] constexpr FSize operator/ (const FSize& rhs) const {
			return FSize { width / rhs.width, height / rhs.height };
		}
		[[nodiscard]] constexpr FSize operator/ (const int32_t rhs) const {
			return FSize { width / rhs, height / rhs };
		}
		[[nodiscard]] constexpr FSize operator- () const {
			return FSize { -width, -height };
		}
		[[nodiscard]] constexpr bool operator== (const FSize& rhs) const {
			return width == rhs.width && height == rhs.height;
		}
		[[nodiscard]] constexpr bool operator!= (const FSize& rhs) const {
			return width != rhs.width || height != rhs.height;
		}

		FSize& operator+= (const FSize& rhs) {
			width += rhs.width;
			height += rhs.height;
			return *this;
		}
		FSize& operator-= (const FSize& rhs) {
			width -= rhs.width;
			height -= rhs.height;
			return *this;
		}
		FSize& operator*= (const FSize& rhs) {
			width *= rhs.width;
			height *= rhs.height;
			return *this;
		}
		FSize& operator*= (const int32_t rhs) {
			width *= rhs;
			height *= rhs;
			return *this;
		}
		FSize& operator/= (const FSize& rhs) {
			width /= rhs.width;
			height /= rhs.height;
			return *this;
		}
		FSize& operator/= (const int32_t rhs) {
			width /= rhs;
			height /= rhs;
			return *this;
		}
	};

	struct Rect {
		Point origin;
		Size size;

		constexpr Rect(const Point& origin, const Size& size) : origin(origin), size(size) { }
		constexpr Rect(const int32_t x, const int32_t y, const int32_t width, const int32_t height) : origin(x, y), size(width, height) { }

		[[nodiscard]] constexpr Rect operator+ (const Point& offset) const {
			return Rect { origin + offset, size };
		}
		[[nodiscard]] constexpr Rect operator- (const Point& offset) const {
			return Rect { origin - offset, size };
		}
		[[nodiscard]] constexpr Rect operator+ (const Size& offset) const {
			return Rect { origin, size + offset };
		}
		[[nodiscard]] constexpr Rect operator- (const Size& offset) const {
			return Rect { origin, size - offset };
		}
		[[nodiscard]] constexpr Rect operator* (const Size& offset) const {
			return Rect { origin, size * offset };
		}
		[[nodiscard]] constexpr Rect operator/ (const Size& offset) const {
			return Rect { origin, size / offset };
		}
		[[nodiscard]] constexpr Rect operator* (const int32_t offset) const {
			return Rect { origin, size * offset };
		}
		[[nodiscard]] constexpr Rect operator/ (const int32_t offset) const {
			return Rect { origin, size / offset };
		}

		[[nodiscard]] constexpr bool operator== (const Rect& other) const {
			return origin == other.origin && size == other.size;
		}
		[[nodiscard]] constexpr bool operator!= (const Rect& other) const {
			return origin != other.origin || size != other.size;
		}
	};
	struct FRect {
		FPoint origin;
		FSize size;

		constexpr FRect(const FPoint& origin, const FSize& size) : origin(origin), size(size) { }
		constexpr FRect(const float x, const float y, const float width, const float height) : origin(x, y), size(width, height) { }

		[[nodiscard]] constexpr FRect operator+ (const FPoint& offset) const {
			return FRect { origin + offset, size };
		}
		[[nodiscard]] constexpr FRect operator- (const FPoint& offset) const {
			return FRect { origin - offset, size };
		}
		[[nodiscard]] constexpr FRect operator+ (const FSize& offset) const {
			return FRect { origin, size + offset };
		}
		[[nodiscard]] constexpr FRect operator- (const FSize& offset) const {
			return FRect { origin, size - offset };
		}
		[[nodiscard]] constexpr FRect operator* (const FSize& offset) const {
			return FRect { origin, size * offset };
		}
		[[nodiscard]] constexpr FRect operator/ (const FSize& offset) const {
			return FRect { origin, size / offset };
		}
		[[nodiscard]] constexpr FRect operator* (const int32_t offset) const {
			return FRect { origin, size * offset };
		}
		[[nodiscard]] constexpr FRect operator/ (const int32_t offset) const {
			return FRect { origin, size / offset };
		}

		[[nodiscard]] constexpr bool operator== (const FRect& other) const {
			return origin == other.origin && size == other.size;
		}
		[[nodiscard]] constexpr bool operator!= (const FRect& other) const {
			return origin != other.origin || size != other.size;
		}
	};

	struct Vector3 {
		float x, y, z;

		constexpr Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
		constexpr Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}

		[[nodiscard]] constexpr float length() const { return std::sqrt(x * x + y * y + z * z); }
		[[nodiscard]] constexpr float length_square() const { return x * x + y * y + z * z; }
		[[nodiscard]] inline std::string to_string() const { return std::format("{}, {}, {}", x, y, z); }
		[[nodiscard]] constexpr float dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
		[[nodiscard]] constexpr float distance(const Vector3& other) const { return (other - *this).length(); }
		[[nodiscard]] constexpr Vector3 normalized() const { return *this / length(); }
		[[nodiscard]] constexpr std::tuple<float, float, float> items() const { return std::make_tuple(x, y, z); }
		[[nodiscard]] constexpr Vector3 cross(const Vector3& other) const {
			return Vector3 {
				x * other.y - y * other.x,
				y * other.z - z * other.y,
				z * other.x - x * other.z
			};
		}

		static Vector3 Min(const Vector3& a, const Vector3& b);
		static Vector3 Max(const Vector3& a, const Vector3& b);
		static Vector3 Clamp(const Vector3& x, const Vector3& min, const Vector3& max);
		static Vector3 Lerp(const Vector3& x, const Vector3& y, float t);

		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;
		static const Vector3 ONE;
		static const Vector3 ZERO;

		[[nodiscard]] constexpr Vector3 operator+ (const Vector3& rhs) const {
			return Vector3 { x + rhs.x, y + rhs.y, z + rhs.z };
		}
		[[nodiscard]] constexpr Vector3 operator- (const Vector3& rhs) const {
			return Vector3 { x - rhs.x, y - rhs.y, z - rhs.z };
		}
		[[nodiscard]] constexpr Vector3 operator* (const Vector3& rhs) const {
			return Vector3 { x * rhs.x, y * rhs.y, z * rhs.z };
		}
		[[nodiscard]] constexpr Vector3 operator* (const float rhs) const {
			return Vector3 { x * rhs, y * rhs, z * rhs };
		}
		[[nodiscard]] constexpr Vector3 operator/ (const Vector3& rhs) const {
			return Vector3 { x / rhs.x, y / rhs.y, z / rhs.z };
		}
		[[nodiscard]] constexpr Vector3 operator/ (const float rhs) const {
			return Vector3 { x / rhs, y / rhs, z / rhs };
		}
		[[nodiscard]] constexpr Vector3 operator- () const {
			return Vector3 { -x, -y, -z };
		}
		[[nodiscard]] constexpr bool operator== (const Vector3& rhs) const {
			return x == rhs.x && y == rhs.y && z == rhs.z;
		}
		[[nodiscard]] constexpr bool operator!= (const Vector3& rhs) const {
			return x != rhs.x || y != rhs.y || z != rhs.z;
		}

		Vector3& operator+= (const Vector3& rhs) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}
		Vector3& operator-= (const Vector3& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}
		Vector3& operator*= (const Vector3& rhs) {
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}
		Vector3& operator*= (const float rhs) {
			x *= rhs;
			y *= rhs;
			z *= rhs;
			return *this;
		}
		Vector3& operator/= (const Vector3& rhs) {
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}
		Vector3& operator/= (const float rhs) {
			x /= rhs;
			y /= rhs;
			z /= rhs;
			return *this;
		}

		constexpr float operator[] (const int index) const {
			if (index == 0) return x;
			if (index == 1) return y;
			if (index == 2) return z;
			throw std::out_of_range("Index out of range");
		}
		float& operator[] (const int index) {
			if (index == 0) return x;
			if (index == 1) return y;
			if (index == 2) return z;
			throw std::out_of_range("Index out of range");
		}
	};

	struct Vector4 {
		float x, y, z, w;

		constexpr Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
		constexpr Vector4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) { }

		[[nodiscard]] constexpr float length() const { return std::sqrt(x * x + y * y + z * z + w * w); }
		[[nodiscard]] constexpr float length_square() const { return x * x + y * y + z * z + w * w; }
		[[nodiscard]] inline std::string to_string() const { return std::format("{}, {}, {}, {}", x, y, z, w); }
		[[nodiscard]] constexpr float dot(const Vector4& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }
		[[nodiscard]] constexpr float distance(const Vector4& other) const { return (other - *this).length(); }
		[[nodiscard]] constexpr Vector4 normalized() const { return *this / length(); }
		[[nodiscard]] constexpr std::tuple<float, float, float, float> items() const { return std::make_tuple(x, y, z, w); }

		static Vector4 Min(const Vector4& a, const Vector4& b);
		static Vector4 Max(const Vector4& a, const Vector4& b);
		static Vector4 Clamp(const Vector4& x, const Vector4& min, const Vector4& max);
		static Vector4 Lerp(const Vector4& x, const Vector4& y, float t);

		static const Vector4 UNIT_X;
		static const Vector4 UNIT_Y;
		static const Vector4 UNIT_Z;
		static const Vector4 UNIT_W;
		static const Vector4 ONE;
		static const Vector4 ZERO;

		[[nodiscard]] constexpr Vector4 operator+ (const Vector4& rhs) const {
			return Vector4 { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
		}
		[[nodiscard]] constexpr Vector4 operator- (const Vector4& rhs) const {
			return Vector4 { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
		}
		[[nodiscard]] constexpr Vector4 operator* (const Vector4& rhs) const {
			return Vector4 { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w };
		}
		[[nodiscard]] constexpr Vector4 operator* (const float rhs) const {
			return Vector4 { x * rhs, y * rhs, z * rhs, w * rhs };
		}
		[[nodiscard]] constexpr Vector4 operator/ (const Vector4& rhs) const {
			return Vector4 { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w };
		}
		[[nodiscard]] constexpr Vector4 operator/ (const float rhs) const {
			return Vector4 { x / rhs, y / rhs, z / rhs, w / rhs };
		}
		[[nodiscard]] constexpr Vector4 operator- () const {
			return Vector4 { -x, -y, -z, -w };
		}
		[[nodiscard]] constexpr bool operator== (const Vector4& rhs) const {
			return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
		}
		[[nodiscard]] constexpr bool operator!= (const Vector4& rhs) const {
			return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
		}

		Vector4& operator+= (const Vector4& rhs) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}
		Vector4& operator-= (const Vector4& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}
		Vector4& operator*= (const Vector4& rhs) {
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w *= rhs.w;
			return *this;
		}
		Vector4& operator*= (const float rhs) {
			x *= rhs;
			y *= rhs;
			z *= rhs;
			w *= rhs;
			return *this;
		}
		Vector4& operator/= (const Vector4& rhs) {
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}
		Vector4& operator/= (const float rhs) {
			x /= rhs;
			y /= rhs;
			z /= rhs;
			w /= rhs;
			return *this;
		}

		constexpr float operator[] (const int index) const {
			if (index == 0) return x;
			if (index == 1) return y;
			if (index == 2) return z;
			if (index == 3) return w;
			throw std::out_of_range("Index out of range");
		}
		float& operator[] (const int index) {
			if (index == 0) return x;
			if (index == 1) return y;
			if (index == 2) return z;
			if (index == 3) return w;
			throw std::out_of_range("Index out of range");
		}
	};

	struct AxisAngle {
		Vector3 axis;
		float angle;

		constexpr AxisAngle(const Vector3& axis, const float angle) : axis(axis), angle(angle) { }
	};

	struct EulerAngles {
		float yaw, pitch, roll;

		constexpr EulerAngles() : yaw(0.0f), pitch(0.0f), roll(0.0f) { }
		constexpr EulerAngles(const float yaw, const float pitch, const float roll) : yaw(yaw), pitch(pitch), roll(roll) { }
		[[nodiscard]] constexpr std::tuple<float, float, float> items() const { return std::make_tuple(yaw, pitch, roll); }
	};

	struct Quaternion {
		float x, y, z, w;

		constexpr Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { }
		constexpr Quaternion(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) { }
		constexpr Quaternion(const Vector4& value) : x(value.x), y(value.y), z(value.z), w(value.w) { } // NOLINT: Implicit conversion intended

		[[nodiscard]] constexpr std::tuple<float, float, float, float> items() const { return std::make_tuple(x, y, z, w); }
		[[nodiscard]] constexpr EulerAngles to_euler() const {
			EulerAngles euler;

			// roll (x-axis rotation)
			const float sinr_cosp = 2.0f * (w * x + y * z);
			const float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
			euler.roll = std::atan2(sinr_cosp, cosr_cosp);

			// pitch (y-axis rotation)
			const float sinp = 2.0f * (w * y - z * x);
			if (fabsf(sinp) >= 1) {
				euler.pitch = copysignf(M_PI_2, sinp); // use 90 degrees if out of range
			} else {
				euler.pitch = asinf(sinp);
			}

			// yaw (z-axis rotation)
			const float siny_cosp = 2.0f * (w * z + x * y);
			const float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
			euler.yaw = std::atan2(siny_cosp, cosy_cosp);

			return euler;
		}
		[[nodiscard]] constexpr AxisAngle to_axis_angle() const {
			const float sqww = std::sqrt(1.0f - w * w);
			const float angle = 2.0f * std::acos(w);
			if (sqww != 0.0f) {
				return AxisAngle {
					Vector3 {
						x / sqww,
						y / sqww,
						z / sqww
					}.normalized(),
					angle
				};
			}
			return AxisAngle { Vector3::ZERO, angle };
		}

		[[nodiscard]] static constexpr Quaternion FromEuler(const EulerAngles& euler) {
			const float cy = std::cos(euler.yaw * 0.5f);
			const float sy = std::sin(euler.yaw * 0.5f);
			const float cp = std::cos(euler.pitch * 0.5f);
			const float sp = std::sin(euler.pitch * 0.5f);
			const float cr = std::cos(euler.roll * 0.5f);
			const float sr = std::sin(euler.roll * 0.5f);

			return Quaternion {
				sr * cp * cy - cr * sp * sy,
				cr * sp * cy + sr * cp * sy,
				cr * cp * sy - sr * sp * cy,
				cr * cp * cy + sr * sp * sy
			};
		}
		[[nodiscard]] constexpr static Quaternion FromAxisAngle(const AxisAngle& aa) {
			const float sa = std::sin(aa.angle * 0.5f);
			const float ca = std::cos(aa.angle * 0.5f);
			const Vector3 axis = Vector3(aa.axis).normalized();

			return Quaternion { axis.x * sa, axis.y * sa, axis.z * sa, ca };
		}

		static const Quaternion IDENTITY;

		[[nodiscard]] constexpr Quaternion operator+ (const Quaternion& rhs) const {
			return Quaternion { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
		}
		[[nodiscard]] constexpr Quaternion operator- (const Quaternion& rhs) const {
			return Quaternion { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
		}
		[[nodiscard]] constexpr Quaternion operator* (const Quaternion& rhs) const {
			return Quaternion { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w };
		}
		[[nodiscard]] constexpr Quaternion operator* (const float rhs) const {
			return Quaternion { x * rhs, y * rhs, z * rhs, w * rhs };
		}
		[[nodiscard]] constexpr Quaternion operator/ (const Quaternion& rhs) const {
			return Quaternion { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w };
		}
		[[nodiscard]] constexpr Quaternion operator/ (const float rhs) const {
			return Quaternion { x / rhs, y / rhs, z / rhs, w / rhs };
		}
		[[nodiscard]] constexpr Quaternion operator- () const {
			return Quaternion { -x, -y, -z, -w };
		}
		[[nodiscard]] constexpr bool operator== (const Quaternion& rhs) const {
			return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
		}
		[[nodiscard]] constexpr bool operator!= (const Quaternion& rhs) const {
			return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
		}

		Quaternion& operator+= (const Quaternion& rhs) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}
		Quaternion& operator-= (const Quaternion& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}
		Quaternion& operator*= (const Quaternion& rhs) {
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w *= rhs.w;
			return *this;
		}
		Quaternion& operator*= (const float rhs) {
			x *= rhs;
			y *= rhs;
			z *= rhs;
			w *= rhs;
			return *this;
		}
		Quaternion& operator/= (const Quaternion& rhs) {
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}
		Quaternion& operator/= (const float rhs) {
			x /= rhs;
			y /= rhs;
			z /= rhs;
			w /= rhs;
			return *this;
		}

		operator Vector4() const { return Vector4 { x, y, z, w }; } // NOLINT: Implicit conversion intended
	};

	struct Matrix4 {
		Vector4 column0, column1, column2, column3;

		constexpr Matrix4(const Vector4& column0, const Vector4& column1, const Vector4& column2, const Vector4& column3) :
			column0(column0), column1(column1), column2(column2), column3(column3) { }
		constexpr Matrix4(
			const float i00, const float i01, const float i02, const float i03,
			const float i10, const float i11, const float i12, const float i13,
			const float i20, const float i21, const float i22, const float i23,
			const float i30, const float i31, const float i32, const float i33
		) : column0(i00, i01, i02, i03), column1(i10, i11, i12, i13), column2(i20, i21, i22, i23), column3(i30, i31, i32, i33) { }
		constexpr Matrix4() : Matrix4(Vector4::UNIT_X, Vector4::UNIT_Y, Vector4::UNIT_Z, Vector4::UNIT_W) { }

		[[nodiscard]] constexpr Vector4 row(const size_t i) const {
			switch (i) {
				case 0: return Vector4 { column0.x, column1.x, column2.x, column3.x };
				case 1: return Vector4 { column0.y, column1.y, column2.y, column3.y };
				case 2: return Vector4 { column0.z, column1.z, column2.z, column3.z };
				case 3: return Vector4 { column0.w, column1.w, column2.w, column3.w };
				default: throw std::out_of_range("Index out of range");
			}
		}

		[[nodiscard]] constexpr Matrix4 transposed() const {
			return Matrix4 {
				column0.x, column1.x, column2.x, column3.x,
				column0.y, column1.y, column2.y, column3.y,
				column0.z, column1.z, column2.z, column3.z,
				column0.w, column1.w, column2.w, column3.w
			};
		}
		[[nodiscard]] constexpr Matrix4 multiply(const Matrix4& other) const {
			return Matrix4 {
				column0.dot(other.row(0)),
				column0.dot(other.row(1)),
				column0.dot(other.row(2)),
				column0.dot(other.row(3)),

				column1.dot(other.row(0)),
				column1.dot(other.row(1)),
				column1.dot(other.row(2)),
				column1.dot(other.row(3)),

				column2.dot(other.row(0)),
				column2.dot(other.row(1)),
				column2.dot(other.row(2)),
				column2.dot(other.row(3)),

				column3.dot(other.row(0)),
				column3.dot(other.row(1)),
				column3.dot(other.row(2)),
				column3.dot(other.row(3))
			};
		}

		inline float& item(const size_t idx) {
			return column(idx / 4)[static_cast<int>(idx % 4u)];
		}
		[[nodiscard]] constexpr float item(const size_t idx) const {
			return column(idx / 4)[static_cast<int>(idx % 4u)];
		}

		inline Vector4& column(const size_t idx) {
			if (idx == 0) return column0;
			if (idx == 1) return column1;
			if (idx == 2) return column2;
			if (idx == 3) return column3;
			throw std::out_of_range("Index out of range");
		}
		[[nodiscard]] inline Vector4 column(const size_t idx) const {
			if (idx == 0) return column0;
			if (idx == 1) return column1;
			if (idx == 2) return column2;
			if (idx == 3) return column3;
			throw std::out_of_range("Index out of range");
		}

		constexpr Matrix4 operator* (const Matrix4& rhs) const {
			return multiply(rhs);
		}

		constexpr static Matrix4 CreateTranslation(const Vector3& position) {
			return Matrix4 {
				0.0f, 0.0f, 0.0f, position.x,
				0.0f, 0.0f, 0.0f, position.y,
				0.0f, 0.0f, 0.0f, position.z,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}
		constexpr static Matrix4 CreateScale(const Vector3& position) {
			return Matrix4 {
				position.x, 0.0f, 0.0f, 0.0f,
				0.0f, position.y, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, position.z,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}
		constexpr static Matrix4 CreateRotationX(const float angle) {
			return Matrix4 {
				1.0f, 0.0f        , 0.0f        , 0.0f,
				0.0f, cosf(angle) , -sinf(angle), 0.0f,
				0.0f, sinf(angle) , cosf(angle) , 0.0f,
				0.0f, 0.0f        , 0.0f        , 1.0f
			};
		}
		constexpr static Matrix4 CreateRotationY(const float angle) {
			return Matrix4 {
				cosf(angle) , 0.0f, sinf(angle), 0.0f,
				0.0f        , 1.0f, 0.0f       , 0.0f,
				-sinf(angle), 0.0f, cosf(angle), 0.0f,
				0.0f        , 0.0f, 0.0f       , 1.0f
			};
		}
		constexpr static Matrix4 CreateRotationZ(const float angle) {
			return Matrix4 {
				cosf(angle) , -sinf(angle), 0.0f, 0.0f,
				sinf(angle), cosf(angle), 0.0f, 0.0f,
				0.0f        , 0.0f       , 1.0f, 0.0f,
				0.0f        , 0.0f       , 0.0f, 1.0f
			};
		}
		constexpr static Matrix4 CreateFromQuaternion(const Quaternion& quat) {
			Matrix4 result = IDENTITY;

			const float xx = quat.x * quat.x;
			const float xy = quat.x * quat.y;
			const float xz = quat.x * quat.z;
			const float xw = quat.x * quat.w;

			const float yy = quat.y * quat.y;
			const float yz = quat.y * quat.z;
			const float yw = quat.y * quat.w;

			const float zz = quat.z * quat.z;
			const float zw = quat.z * quat.w;

			const float ww = quat.w * quat.w;

			const float s2 = 2.0f / (xx + yy + zz + ww);

			result.column0.x = 1.0f - s2 * (yy + zz);
			result.column0.y = s2 * (xy - zw);
			result.column0.z = s2 * (xz + yw);

			result.column1.x = s2 * (xy + zw);
			result.column1.y = 1.0f - s2 * (xx + zz);
			result.column1.z = s2 * (yz - xw);

			result.column2.x = s2 * (xz - yw);
			result.column2.y = s2 * (yz + yw);
			result.column2.z = 1.0f - s2 * (xx + yy);

			return result;
		}
		constexpr static Matrix4 CreateFromAxisAngle(const AxisAngle& aa) {
			Matrix4 result = IDENTITY;
			const Vector3 axis = aa.axis.normalized();
			const float c = cosf(aa.angle);
			const float s = sinf(aa.angle);
			const float t = 1.0f - c;

			result.column0.x = axis.x * axis.x * t;
			result.column1.y = axis.y * axis.y * t;
			result.column2.z = axis.z * axis.z * t;

			float a = axis.x * axis.y * t;
			float b = axis.z * s;
			result.column1.x = a + b;
			result.column0.y = a - b;

			a = axis.x * axis.z * t;
			b = axis.y * s;
			result.column2.x = a - b;
			result.column0.z = a + b;

			a = axis.y * axis.z * t;
			b = axis.x * s;
			result.column2.y = a + b;
			result.column1.z = a - b;
			return result;
		}
		constexpr static Matrix4 CreateFromEuler(const EulerAngles& e) {
			return CreateFromQuaternion(Quaternion::FromEuler(e));
		}
		constexpr static Matrix4 CreateOrtho(const float left, const float right, const float bottom, const float top, const float near, const float far) {
			Matrix4 result = IDENTITY;

			result.item(0)  = 2.0f / (right - left);
			result.item(5)  = 2.0f / (top - bottom);
			result.item(10) = -2.0f / (far - near);
			result.item(3)  = -(right + left) / (right - left);
			result.item(7)  = -(top + bottom) / (top - bottom);
			result.item(11) = -(far + near) / (far - near);
			result.item(15) = 1.0f;

			return result;
		}

		constexpr static Matrix4 CreatePerspective(const float fovy, const float aspect, const float near, const float far) {
			Matrix4 result = IDENTITY;
			const float f = 1.0f / tanf(Math::Deg2Rad(fovy) * 0.5f);
			const float nf = 1.0f / (near - far);

			result.item(0)  = f / aspect;
			result.item(5)  = f;
			result.item(10) = (far + near) * nf;
			result.item(11) = (2.0f * far * near) * nf;
			result.item(14) = -1.0f;
			result.item(15) = 0.0f;

			return result;
		}

		static const Matrix4 IDENTITY;
	};

	enum class ColorFormat {
		Rgb,
		Rgba,
		Hsl,
		Hsla,
		Hex,
		Vector
	};

	struct Color {
		float r, g, b, a;

		constexpr Color() : r(0), g(0), b(0), a(1) {}
		constexpr Color(const float r, const float g, const float b, const float a = 1.0f) : r(r), g(g), b(b), a(a) {}

		[[nodiscard]] constexpr std::tuple<float, float, float, float> items() const { return std::make_tuple(r, g, b, a); }
		[[nodiscard]] constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> to_bytes() const {
			return std::make_tuple(
				static_cast<uint8_t>(r * 255.0f),
				static_cast<uint8_t>(g * 255.0f),
				static_cast<uint8_t>(b * 255.0f),
				static_cast<uint8_t>(a * 255.0f)
			);
		}
		[[nodiscard]] constexpr uint32_t to_rgba() const {
			auto [ r, g, b, a ] = to_bytes();
			return r << 24 | g << 16 | b << 8 | a;
		}
		[[nodiscard]] constexpr uint32_t to_argb() const {
			auto [ r, g, b, a ] = to_bytes();
			return r << 16 | g << 8 | b | a << 24;
		}

		[[nodiscard]] constexpr std::tuple<float, float, float, float> to_hsl() const {
			const float _max = Math::Max(r, Math::Max(g, b));
			const float _min = Math::Min(r, Math::Max(g, b));
			float h = 0.0f, s = 0.0f;
			const float l = (_min + _max) / 2;

			if (_max == _min) {
				h = 0.0f;
				s = 0.0f;
			} else {
				const float d = _max - _min;
				s = l > 0.5f ? d / (2.0f - _max - _min) : d / (_max + _min);

				if (_max == r) {
					h = (g - b) / d + (g < b ? 6.0f : 0.0f);
				} else if (_max == g) {
					h = (b - r) / d + 2;
				} else if (_max == b) {
					h = (b - r) / d + 4;
				}

				h /= 6;
			}
			return std::make_tuple(h, s, l, a);
		}

		[[nodiscard]] static constexpr Color FromRgba(const uint32_t rgba) {
			return FromRgba((rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF, rgba & 0xFF);
		}
		[[nodiscard]] static constexpr Color FromRgba(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 0xFF) {
			return Color {
				static_cast<float>(r) / 255.0f,
				static_cast<float>(g) / 255.0f,
				static_cast<float>(b) / 255.0f,
				static_cast<float>(a) / 255.0f
			};
		}
		[[nodiscard]] static constexpr Color FromArgb(const uint32_t argb) {
			return FromRgba((argb >> 16) & 0xFF, (argb >> 8) & 0xFF, argb & 0xFF, (argb >> 24) & 0xFF);
		}
		[[nodiscard]] static constexpr Color FromHsl(const float h, const float s, const float l, const float a = 1.0f) {
			const auto ColorHslHueToRgb = [](const float p, const float q, float t) -> float {
				if (t < 0) t += 1;
				if (t > 1) t -= 1;
				if (t < 1.0f / 6.0f) return p + (q - p) * 6 * t;
				if (t < 1.0f / 2.0f) return q;
				if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6;
				return p;
			};

			Color color = { 0.0f, 0.0f, 0.0f, a };

			if (s == 0.0f) {
				color.r = color.g = color.b = l;
			} else {
				const float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
				const float p = 2.0f * l - q;

				color.r = ColorHslHueToRgb(p, q, h + 1.0f / 3.0f);
			}

			return color;
		}
#include "gctk_colors.hpp"
	};

	class Random {
		std::mt19937 m_engine;
	public:
		explicit Random(const size_t seed) : m_engine(seed) { }
		Random() : Random(std::chrono::high_resolution_clock::now().time_since_epoch().count()) { }

		template<IntegerType T>
		[[nodiscard]] inline T next_int(const T min, const T max) {
			std::uniform_int_distribution<T> dist(min, max);
			return dist(m_engine);
		}
		template<FloatType T>
		[[nodiscard]] inline T next_float(const T min, const T max) {
			std::uniform_real_distribution<T> dist(min, max);
			return dist(m_engine);
		}
		[[nodiscard]] inline Vector2 next_vector2(const Vector2& min, const Vector2& max) {
			return Vector2 {
				next_float<float>(min.x, max.x),
				next_float<float>(min.y, max.y)
			};
		}
		[[nodiscard]] inline Vector3 next_vector3(const Vector3& min, const Vector3& max) {
			return Vector3 {
				next_float<float>(min.x, max.x),
				next_float<float>(min.y, max.y),
				next_float<float>(min.z, max.z)
			};
		}
		[[nodiscard]] inline Vector4 next_vector4(const Vector4& min, const Vector4& max) {
			return Vector4 {
				next_float<float>(min.x, max.x),
				next_float<float>(min.y, max.y),
				next_float<float>(min.z, max.z),
				next_float<float>(min.w, max.w)
			};
		}
		[[nodiscard]] inline Vector2 next_in_circle(const float r_min, const float r_max) {
			const auto r = next_float<float>(r_min, r_max);
			const auto angle = next_float<float>(0.0f, Math::TwoPi<float>);
			return Vector2 { r * Math::Cos(angle), r * Math::Sin(angle) };
		}
		[[nodiscard]] inline Vector3 next_in_sphere(const float r_min, const float r_max) {
			const auto r = next_float<float>(r_min, r_max);
			const auto a = next_float<float>(0.0f, Math::TwoPi<float>);
			const auto b = next_float<float>(0.0f, Math::TwoPi<float>);
			return Vector3 {
				r * Math::Sin(a) * Math::Cos(b),
				r * Math::Sin(a) * Math::Sin(b),
				r * Math::Cos(a)
			};
		}
		inline void seed() { seed(std::chrono::high_resolution_clock::now().time_since_epoch().count()); }
		inline void seed(const size_t seed) { m_engine.seed(seed); }
	};
}

template<>
struct std::formatter<gctk::Vector2, char> {
private:
	std::string separator = ", ";
public:
	template<class ParseContext>
	constexpr typename ParseContext::iterator parse(ParseContext& ctx) {
		auto it = ctx.begin();
		separator = "";
		while (it != ctx.end()) {
			separator += *it;
		}
		return it;
	}
	template<class FmtContext>
	typename FmtContext::iterator format(const gctk::Vector2& v, FmtContext& ctx) const {
		return std::format_to(ctx.out(), "{}{}{}", v.x, separator, v.y);
	}
};

template<>
struct std::formatter<gctk::Vector3, char> {
private:
	std::string separator = ", ";
public:
	template<class ParseContext>
	constexpr typename ParseContext::iterator parse(ParseContext& ctx) {
		auto it = ctx.begin();
		separator = "";
		while (it != ctx.end()) {
			separator += *it;
		}
		return it;
	}
	template<class FmtContext>
	typename FmtContext::iterator format(const gctk::Vector3& v, FmtContext& ctx) const {
		return std::format_to(ctx.out(), "{}{}{}{}{}", v.x, separator, v.y, separator, v.z);
	}
};

template<>
struct std::formatter<gctk::Vector4, char> {
private:
	std::string separator = ", ";
public:
	template<class ParseContext>
	constexpr typename ParseContext::iterator parse(ParseContext& ctx) {
		auto it = ctx.begin();
		separator = "";
		while (it != ctx.end()) {
			separator += *it;
		}
		return it;
	}
	template<class FmtContext>
	typename FmtContext::iterator format(const gctk::Vector4& v, FmtContext& ctx) const {
		return std::format_to(ctx.out(), "{}{}{}{}{}{}{}", v.x, separator, v.y, separator, v.z, separator, v.w);
	}
};

template<>
struct std::formatter<gctk::Matrix4, char> {
private:
	std::string separator = ", ";
public:
	template<class ParseContext>
	constexpr typename ParseContext::iterator parse(ParseContext& ctx) {
		auto it = ctx.begin();
		separator = "";
		while (it != ctx.end()) {
			separator += *it;
		}
		return it;
	}
	template<class FmtContext>
	typename FmtContext::iterator format(const gctk::Matrix4& m, FmtContext& ctx) const {
		return std::format_to(
			ctx.out(),
			"[{}{}{}{}{}{}{}]\n"
			"[{}{}{}{}{}{}{}]\n"
			"[{}{}{}{}{}{}{}]\n"
			"[{}{}{}{}{}{}{}]",
			m.column0.x, separator, m.column0.y, separator, m.column0.z, separator, m.column0.w,
			m.column1.x, separator, m.column1.y, separator, m.column1.z, separator, m.column1.w,
			m.column2.x, separator, m.column2.y, separator, m.column2.z, separator, m.column2.w,
			m.column3.x, separator, m.column3.y, separator, m.column3.z, separator, m.column3.w
		);
	}
};

template<>
struct std::formatter<gctk::Color, char> {
private:
	gctk::ColorFormat color_format = gctk::ColorFormat::Vector;
	bool has_prefix = false;
public:
	template<class ParseContext>
	constexpr typename ParseContext::iterator parse(ParseContext& ctx) {
		auto it = ctx.begin();

		std::string fmt;
		while (it != ctx.end()) {
			fmt += *it;
		}

		if (fmt.size() > 1 && fmt.starts_with('#')) {
			fmt.erase(0, 1);
			has_prefix = true;
		}

		for (auto& c : fmt) {
			c = static_cast<char>(std::tolower(c));
		}

		if (fmt == "rgb") {
			color_format = gctk::ColorFormat::Rgb;
		} else if (fmt == "rgba") {
			color_format = gctk::ColorFormat::Rgba;
		} else if (fmt == "hsl") {
			color_format = gctk::ColorFormat::Hsl;
		} else if (fmt == "hsla") {
			color_format = gctk::ColorFormat::Hsla;
		} else if (fmt == "x") {
			color_format = gctk::ColorFormat::Hex;
		} else {
			color_format = gctk::ColorFormat::Vector;
		}

		return it;
	}

	template<class FmtContext>
	typename FmtContext::iterator format(const gctk::Color& c, FmtContext& ctx) const {
		switch (color_format) {
			case gctk::ColorFormat::Rgb: {
				std::string prefix;
				std::string suffix;

				if (has_prefix) {
					prefix = "rgb(";
					suffix = ")";
				}

				return std::format_to(ctx.out(), "{}{}, {}, {}{}",
					prefix,
					static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255),
					suffix
				);
			}
			case gctk::ColorFormat::Rgba: {
				std::string prefix;
				std::string suffix;

				if (has_prefix) {
					prefix = "rgba(";
					suffix = ")";
				}

				return std::format_to(ctx.out(), "{}{}, {}, {}, {}{}",
					prefix,
					static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255), static_cast<int>(c.a * 255),
					suffix
				);
			}
			case gctk::ColorFormat::Hex: return std::format_to(ctx.out(), has_prefix ? "#{:X}" : "{:X}", c.to_rgba());
			case gctk::ColorFormat::Hsl: {
				std::string prefix;
				std::string suffix;

				if (has_prefix) {
					prefix = "hsl(";
					suffix = ")";
				}

				auto [ h, s, l, _ ] = c.to_hsl();
				return std::format_to(ctx.out(), "{}{}, {}%, {}%{}", prefix, static_cast<int>(h), static_cast<int>(s * 100), static_cast<int>(l * 100), suffix);
			}
			case gctk::ColorFormat::Hsla: {
				std::string prefix;
				std::string suffix;

				if (has_prefix) {
					prefix = "hsla(";
					suffix = ")";
				}

				auto [ h, s, l, a ] = c.to_hsl();
				return std::format_to(ctx.out(), "{}{}, {}%, {}%, {}{}", prefix, static_cast<int>(h), static_cast<int>(s * 100), static_cast<int>(l * 100), a, suffix);
			}
			default: return std::format_to(ctx.out(), "{}, {}, {}, {}", c.r, c.g, c.b, c.a);
		}
	}
};
