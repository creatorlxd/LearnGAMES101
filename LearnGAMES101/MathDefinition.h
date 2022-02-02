#pragma once
#include "Eigen/Dense"
#define PI 3.1415926f
namespace LearnGames
{
	template<uint64_t D>
	using Vector = Eigen::Matrix<float, 1, D>;

	using Matrix2 = Eigen::Matrix2f;
	using Matrix4 = Eigen::Matrix4f;
	using Vector4 = Vector<4>;
	using Vector2 = Vector<2>;

	Vector2 Vector4ToVector2(const Vector4& v);
	float Cross2(const Vector2& v1, const Vector2& v2);

	template<uint64_t Dim>
	inline Vector<Dim> Interpolat(const Vector4& p, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector<Dim>& d0, const Vector<Dim>& d1, const Vector<Dim>& d2)
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
		Vector<Dim> de01 = d1 - d0;
		Vector<Dim> de12 = d2 - d1;
		return i * de01 + j * de12 + d0;
	}
}