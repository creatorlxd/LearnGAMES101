#include "MathDefinition.h"

using namespace LearnGames;

Vector2 LearnGames::Vector4ToVector2(const Vector4& v)
{
	return Vector2(v(0), v(1));
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
