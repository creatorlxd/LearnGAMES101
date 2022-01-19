#pragma once
#include "Eigen/Dense"
#define PI 3.1415926f
namespace LearnGames
{
	using Matrix4 = Eigen::Matrix4f;
	using Vector4 = Eigen::Matrix<float, 1, 4>;
	using Vector2 = Eigen::Matrix<float, 1, 2>;

	Vector2 Vector4ToVector2(const Vector4& v);
	float Cross2(const Vector2& v1, const Vector2& v2);
}