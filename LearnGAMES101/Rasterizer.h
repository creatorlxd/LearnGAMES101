#pragma once
#include "MathDefinition.h"
#include "InputData.h"
#include "Picture.h"

namespace LearnGames
{
	enum class RasterizationState
	{
		Solid = 0,
		Wireframe = 1
	};

	struct Pixel
	{
		uint64_t m_X = 0;
		uint64_t m_Y = 0;
		float m_Z = 0;
		float m_MSAAValue = 1.0f;
	};

	std::vector<std::vector<Pixel>> Rasterize(uint64_t width, uint64_t height, const std::vector<Vertex>& vertices, const std::vector<uint64_t>& indices, RasterizationState state, uint64_t msaa_x = 1, uint64_t msaa_y = 1);
}