#include "ViewPort.h"

LearnGames::Matrix4 LearnGames::ViewPort::GetViewPortMatrix() const
{
	Matrix4 re = Matrix4::Identity();
	re(0, 0) = (float)m_Width / 2.0f;
	re(1, 1) = (float)m_Height / -2.0f;
	re(3, 0) = (float)m_Width / 2.0f + (float)m_StartX;
	re(3, 1) = (float)m_Height / 2.0f + (float)m_StartY;
	return re;
}