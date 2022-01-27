#include "Output.h"

Picture LearnGames::DrawTriangles(uint64_t width, uint64_t height, const std::vector<std::vector<Pixel>>& data, RGBA background_color, uint64_t msaa_way)
{
	assert(msaa_way > 0);
	Picture re(width, height);

	std::vector<std::vector<float>> zbuffer(height, std::vector<float>(width * msaa_way, 2.0f));
	std::vector<std::vector<RGBA>> color_buf(height, std::vector<RGBA>(width * msaa_way, background_color));

	for (auto& triangle : data)
	{
		for (auto& p : triangle)
		{
			uint64_t cnt = 0;
			Vector2 pixel_point = Vector2::Zero();
			for (auto& mp : p.m_MSAAPoints)
			{
				pixel_point += GetMSAAPoint(msaa_way, mp.first);
				++cnt;
			}
			pixel_point /= (float)cnt;
			pixel_point(0) += (float)p.m_X;
			pixel_point(1) += (float)p.m_Y;
			//todo : do pixel shading for pixel point
			RGBA color(255, 0, 0, 255);
			for (auto& mp : p.m_MSAAPoints)
			{
				if (zbuffer[p.m_Y][p.m_X * msaa_way + mp.first] > mp.second)
				{
					zbuffer[p.m_Y][p.m_X * msaa_way + mp.first] = mp.second;
					color_buf[p.m_Y][p.m_X * msaa_way + mp.first] = color;
				}
			}
		}
	}
	for (uint64_t i = 0; i < width; ++i)
	{
		for (uint64_t j = 0; j < height; ++j)
		{
			uint64_t R = 0, G = 0, B = 0, A = 0;
			for (uint64_t idx = 0; idx < msaa_way; ++idx)
			{
				R += color_buf[j][i * msaa_way + idx].m_R;
				G += color_buf[j][i * msaa_way + idx].m_G;
				B += color_buf[j][i * msaa_way + idx].m_B;
				A += color_buf[j][i * msaa_way + idx].m_A;
			}
			R /= msaa_way;
			G /= msaa_way;
			B /= msaa_way;
			A /= msaa_way;
			re.DrawPoint(i, j, RGBA(R, G, B, A));
		}
	}
	return re;
}
