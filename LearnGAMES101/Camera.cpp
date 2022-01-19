#include "Camera.h"
#include <cmath>

LearnGames::Matrix4 LearnGames::Camera::GetViewMatrix() const
{
	Matrix4 re;
	Vector4 x_axis = m_Up.cross3(m_Look);
	for (int i = 0; i < 3; ++i)
	{
		re(i, 0) = x_axis(i);
		re(i, 1) = m_Up(i);
		re(i, 2) = m_Look(i);
		re(i, 3) = 0.0f;
	}
	for (int i = 0; i < 3; ++i)
	{
		re(3, i) = -1.0f * m_Position(i);
	}
	re(3, 3) = 1.0f;
	return re;
}

LearnGames::Matrix4 LearnGames::Camera::GetProjectionMatrix() const
{
	Matrix4 re = Matrix4::Zero();
	float whratio = (float)m_Width / (float)m_Height;
	re(0, 0) = 1.0f / whratio * tan(m_FOVY / 2.0f);
	re(1, 1) = 1.0f / tan(m_FOVY / 2.0f);
	re(2, 2) = m_FarZ / (m_FarZ - m_NearZ);
	re(3, 2) = (-1.0f * m_NearZ * m_FarZ) / (m_FarZ - m_NearZ);
	re(2, 3) = 1;
	return re;
}

LearnGames::Matrix4 LearnGames::Camera::GetViewPortMatrix() const
{
	Matrix4 re = Matrix4::Identity();
	re(0, 0) = (float)m_Width / 2.0f;
	re(1, 1) = (float)m_Height / 2.0f;
	re(3, 0) = (float)m_Width / 2.0f;
	re(3, 1) = (float)m_Height / 2.0f;
	return re;
}
