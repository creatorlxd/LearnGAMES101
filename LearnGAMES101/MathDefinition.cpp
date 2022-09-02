#include "MathDefinition.h"
#include <cmath>

using namespace LearnGames;

Vector2 LearnGames::Vector4ToVector2(const Vector4& v)
{
	return Vector2(v(0), v(1));
}

Vector3 LearnGames::Vector4ToVector3(const Vector4& v)
{
	return Vector3(v(0), v(1), v(2));
}

float LearnGames::Cross2(const Vector2& v1, const Vector2& v2)
{
	return v1(0) * v2(1) - v1(1) * v2(0);
}

std::pair<float, float> LearnGames::HalfInterpolat(const Vector4& p, const Vector4& p0, const Vector4& p1, const Vector4& p2)
{
	Vector4 _p = p - p0;
	Vector4 e01 = p1 - p0;
	Vector4 e12 = p2 - p1;
	// _p = i*e01 + j*e12
	Matrix2 D;
	D << e01(0), e12(0), e01(1), e12(1);
	Matrix2 D1;
	D1 << _p(0), e12(0), _p(1), e12(1);
	Matrix2 D2;
	D2 << e01(0), _p(0), e01(1), _p(1);
	float D_d = D.determinant();
	float i = D1.determinant() / D_d;
	float j = D2.determinant() / D_d;
	return std::make_pair(i, j);
}

Vector4 LearnGames::GetNormalVector(const Vector4& p0, const Vector4& p1, const Vector4& p2)
{
	auto re = ((p1 - p0).cross3(p2 - p1)).normalized();
	re(3) = 1.0f;
	return re;
}

Matrix4 LearnGames::GetMoveTransform(float x, float y, float z)
{
	Matrix4 re = Matrix4::Identity();
	re(3, 0) = x;
	re(3, 1) = y;
	re(3, 2) = z;
	return re;
}

Matrix4 LearnGames::GetRotationXTransform(float radian)
{
	Matrix4 re = Matrix4::Identity();
	re(1, 1) = cos(radian);
	re(1, 2) = sin(radian);
	re(2, 1) = -1.0f * sin(radian);
	re(2, 2) = cos(radian);
	return re;
}

Matrix4 LearnGames::GetRotationYTransform(float radian)
{
	Matrix4 re = Matrix4::Identity();
	re(0, 0) = cos(radian);
	re(0, 2) = -1.0f * sin(radian);
	re(2, 0) = sin(radian);
	re(2, 2) = cos(radian);
	return re;
}

Matrix4 LearnGames::GetRotationZTransform(float radian)
{
	Matrix4 re = Matrix4::Identity();
	re(0, 0) = cos(radian);
	re(0, 1) = sin(radian);
	re(1, 0) = -1.0f * sin(radian);
	re(1, 1) = cos(radian);
	return re;
}

Matrix4 LearnGames::GetScaleTransform(float x, float y, float z)
{
	Matrix4 re = Matrix4::Identity();
	re(0, 0) = x;
	re(1, 1) = y;
	re(2, 2) = z;
	return re;
}

Vector3 LearnGames::Reflect(const Vector3& v, const Vector3& normal)
{
	return (v + 2.0f * ((-1.0f * v).dot(normal)) * normal).normalized();
}
