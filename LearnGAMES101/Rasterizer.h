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
		std::vector<std::pair<uint64_t, float>> m_MSAAPoints;	//idx,z
	};

	Vector2 GetMSAAPoint(uint64_t msaa_way, uint64_t idx);

	std::vector<std::vector<Pixel>> Rasterize(uint64_t width, uint64_t height, const std::vector<Vertex>& vertices, const std::vector<uint64_t>& indices, RasterizationState state, uint64_t msaa_way = 1);
}