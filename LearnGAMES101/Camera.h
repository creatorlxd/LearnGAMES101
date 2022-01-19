#pragma once
#include <cstdint>
#include "MathDefinition.h"

namespace LearnGames
{
	class Camera
	{
	public:
		Matrix4 GetViewMatrix()const;
		Matrix4 GetProjectionMatrix()const;
	public:
		Vector4 m_Position;
		Vector4 m_Look;
		Vector4 m_Up;
		float m_WHRatio = 1.0f;
		float m_FOVY = PI / 2.0f;
		float m_NearZ = 0.5f;
		float m_FarZ = 1000.0f;
	};
}