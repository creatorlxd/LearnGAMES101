#pragma once
#include "MathDefinition.h"

namespace LearnGames
{
	struct ViewPort
	{
		uint64_t m_Width = 1920;
		uint64_t m_Height = 1080;
		uint64_t m_StartX = 0;
		uint64_t m_StartY = 0;
		Matrix4 GetViewPortMatrix()const;
	};
}