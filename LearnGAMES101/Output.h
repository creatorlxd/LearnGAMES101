#pragma once
#include "Rasterizer.h"

namespace LearnGames
{
	Picture DrawTriangles(uint64_t width, uint64_t height, const std::vector<std::vector<Pixel>>& data, RGBA background_color, uint64_t msaa_way = 1);
}