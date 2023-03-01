	#pragma once

	#define _USE_MATH_DEFINES

	#include <GL/glew.h>
	#include <GL/freeglut.h> 
	#include <iostream>
	#include <math.h>
	#include <random>
	#include <memory>

	struct Vector
	{
	Vector(float pX = 0.f, float pY = 0.f, float pZ = 0.f)
		:
		X{ pX },
		Y{ pY },
		Z{ pZ }
	{

	}

	float length() const noexcept
	{
		return std::sqrt(X * X + Y * Y + Z * Z);
	}

	Vector& operator+=(const Vector& rhs) noexcept
	{
		return *this = *this + rhs;;
	}

	Vector& operator-=(const Vector& rhs) noexcept
	{
		return *this = *this - rhs;;
	}

	Vector operator+(const Vector& rhs) const noexcept
	{
		return { X + rhs.X, Y + rhs.Y, Z + rhs.Z };
	}

	Vector operator-(const Vector& rhs) const noexcept
	{
		return { X - rhs.X, Y - rhs.Y, Z - rhs.Z };
	}

	Vector operator*(float v) const noexcept
	{
		return { X * v, Y * v, Z * v };
	}

	bool operator==(const Vector& rhs) const noexcept
	{
		return (X == rhs.X) && (Y == rhs.Y) && (Z == rhs.Z);
	}

	bool operator!=(const Vector& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	float X;
	float Y;
	float Z;
	};

	inline Vector normalize(const Vector& v)
	{
	return { v.X / v.length(), v.Y / v.length(), v.Z / v.length() };
	}

	inline float dotProduct(const Vector& lhs, const Vector& rhs)
	{
		return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
	}

	inline bool inRange(float min, float max, float v)
	{
	return (min <= v) && (v <= max);
	}

	inline float getAngleBetweenVectors(const Vector& lhs, const Vector& rhs)
	{
	const auto scalarProduct = dotProduct(lhs, rhs);
	const auto lengthsMult = lhs.length() * rhs.length();
	
	return acosf(scalarProduct / lengthsMult) / M_PI * 180;
	
	}

	template<typename T>
	inline T clamp(T min, T max, T val)
	{
	return min > val ? min : val > max ? max : val;
	}

	inline bool isNearlyEqual(float a, float b, float e)
	{
	return std::abs(a - b) < e;
	}

	inline std::ostream& operator<<(std::ostream& os, const Vector& v)
	{
	os << "X: " << v.X << " Y: " << v.Y << " Z: " << v.Z;
	return os;
	}

	struct Color
	{
	Color(float pR = 0.f, float pG = 0.f, float pB = 0.f)
		:
		R{ pR },
		G{ pG },
		B{ pB }
	{

	}

	float R;
	float G;
	float B;
	};

	struct Rotation
	{
	Rotation(float pAngle = 0, const Vector& pDirs = Vector{})
		:
		angle{ pAngle },
		dirs{ pDirs }
	{

	}

	float angle;
	Vector dirs;
	};

	struct Transform
	{
	Transform(const Vector& pTranslation = {},
		const Vector& pScale = {},
		const Rotation& pRotation = {}
	)
		:
		translation{ pTranslation },
		scale{ pScale },
		rotation{ pRotation }
	{

	}

	Vector translation;
	Vector scale;
	Rotation rotation;
	};

	struct Random
	{
	
	float getRandomFloat(float from, float to)
	{
		std::uniform_real_distribution<float> di(from, to);
		return di(mt);
	}

	int getRandomInt(int from, int to)
	{
		std::uniform_int_distribution<int> di(from, to);
		return di(mt);
	}

	static Random& get()
	{
		static Random random;
		return random;
	}

	private:
	Random()
		:
		mt{static_cast<unsigned int>(time(nullptr))}
	{

	}
	
	std::mt19937 mt;
	};

	inline Color getRandomColor()
	{
	const std::vector<Color> colors
	{
		{0.f, 0.f, 1.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 1.f},
		{1.f, 0.f, 0.f},
		{1.f, 0.f, 1.f},
		{1.f, 1.f, 0.f},
		{1.f, 1.f, 1.f}
	};

	return colors[Random::get().getRandomInt(0,colors.size() - 1)];
	}

	inline void writeBitmapString(void* font, const std::string& str)
	{
	for (const auto ch : str) glutBitmapCharacter(font, ch);
	}

	inline bool checkSphereCollision(const Vector& sph1Loc, float r1, const Vector& sph2Loc, float r2)
	{
		const auto diff = sph2Loc - sph1Loc;
		return  (diff.X * diff.X + diff.Y * diff.Y + diff.Z * diff.Z) <= ((r1 + r2) * (r1 + r2));
	}


	template<typename T1, typename T2>
	inline T1 lerp(const T1& a, const T1& b, T2 t)
	{
	return (1 - t) * a + b * t;
	}
