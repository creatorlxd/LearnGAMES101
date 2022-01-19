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

	void Rasterize(Picture& pic_out, const std::vector<Vertex>& vertices, const std::vector<uint64_t>& indices, RasterizationState state);
}