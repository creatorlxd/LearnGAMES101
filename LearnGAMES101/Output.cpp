#include "Output.h"

Picture LearnGames::DrawTriangles(uint64_t width, uint64_t height, const std::vector<std::vector<Pixel>>& data, RGBA background_color)
{
	Picture re(width, height);

	std::vector<std::vector<float>> zbuffer(height, std::vector<float>(width, 2.0f));

	for (uint64_t i = 0; i < width; ++i)
		for (uint64_t j = 0; j < height; ++j)
			re.DrawPoint(i, j, background_color);

	for (auto& triangle : data)
	{
		for (auto& p : triangle)
		{
			if (zbuffer[p.m_Y][p.m_X] > p.m_Z)
			{
				zbuffer[p.m_Y][p.m_X] = p.m_Z;
				//todo add pixel color and uv texture
				RGBA color(255, 0, 0, 255);
				for (int i = 0; i < 3; ++i)
					color.m_Array[i] = (float)color.m_Array[i] * p.m_MSAAValue;
				re.DrawPoint(p.m_X, p.m_Y, color);
			}
		}
	}
	return re;
}
