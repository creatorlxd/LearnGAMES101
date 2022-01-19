#include "Rasterizer.h"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace
{
	const float raster_eps = 0.00001f;

	bool IsPixelInTriangle(uint64_t p_x, uint64_t p_y, const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2)
	{
		float pxf = (float)p_x + 0.5f;
		float pyf = (float)p_y + 0.5f;
		LearnGames::Vector2 e01 = LearnGames::Vector4ToVector2(v1 - v0);
		LearnGames::Vector2 e12 = LearnGames::Vector4ToVector2(v2 - v1);
		LearnGames::Vector2 e20 = LearnGames::Vector4ToVector2(v0 - v2);
		LearnGames::Vector2 e0p = LearnGames::Vector2(pxf - v0(0), pyf - v0(1));
		LearnGames::Vector2 e1p = LearnGames::Vector2(pxf - v1(0), pyf - v1(1));
		LearnGames::Vector2 e2p = LearnGames::Vector2(pxf - v2(0), pyf - v2(1));
		float fbuf = 0.0f;

		if ((fbuf = LearnGames::Cross2(e01, e0p)) < -raster_eps)
			return false;
		if (abs(fbuf) <= raster_eps && e01(1) < 0.0f)
			return false;

		if ((fbuf = LearnGames::Cross2(e12, e1p)) < -raster_eps)
			return false;
		if (abs(fbuf) <= raster_eps && e12(1) < 0.0f)
			return false;

		if ((fbuf = LearnGames::Cross2(e20, e2p)) < -raster_eps)
			return false;
		if (abs(fbuf) <= raster_eps && e20(1) < 0.0f)
			return false;

		return true;
	}

	bool IsPixelOnTriangle(uint64_t p_x, uint64_t p_y, const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2)
	{
		float pxf = (float)p_x + 0.5f;
		float pyf = (float)p_y + 0.5f;
		LearnGames::Vector2 e01 = LearnGames::Vector4ToVector2(v1 - v0);
		LearnGames::Vector2 e12 = LearnGames::Vector4ToVector2(v2 - v1);
		LearnGames::Vector2 e20 = LearnGames::Vector4ToVector2(v0 - v2);
		LearnGames::Vector2 e0p = LearnGames::Vector2(pxf - v0(0), pyf - v0(1));
		LearnGames::Vector2 e1p = LearnGames::Vector2(pxf - v1(0), pyf - v1(1));
		LearnGames::Vector2 e2p = LearnGames::Vector2(pxf - v2(0), pyf - v2(1));

		e01.normalize();
		e12.normalize();
		e20.normalize();

		if (abs(LearnGames::Cross2(e01, e0p)) <= 0.75)
			return true;
		if (abs(LearnGames::Cross2(e12, e1p)) <= 0.75)
			return true;
		if (abs(LearnGames::Cross2(e20, e2p)) <= 0.75)
			return true;

		return false;
	}


	bool IsBackFace(const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2)
	{
		LearnGames::Vector2 e01 = LearnGames::Vector4ToVector2(v1 - v0);
		LearnGames::Vector2 e12 = LearnGames::Vector4ToVector2(v2 - v1);

		return LearnGames::Cross2(e01, e12) < 0.0f;
	}
}

void LearnGames::Rasterize(Picture& pic_out, const std::vector<Vertex>& vertices, const std::vector<uint64_t>& indices, RasterizationState state)
{
	assert(indices.size() % 3 == 0);

	uint64_t width = pic_out.GetWidth();
	uint64_t height = pic_out.GetHeight();

	for (uint64_t i = 0; i < indices.size(); i += 3)
	{
		//back face culling
		if (IsBackFace(vertices[indices[i]].m_Datas[0].first, vertices[indices[i + 1]].m_Datas[0].first, vertices[indices[i + 2]].m_Datas[0].first))
			continue;

		//calculate aabb box
		uint64_t min_x = width, min_y = height;
		uint64_t max_x = 0, max_y = 0;
		for (uint64_t j = i; j < i + 3; ++j)
		{
			uint64_t x = floor(vertices[indices[j]].m_Datas[0].first(0) - raster_eps);
			uint64_t y = floor(vertices[indices[j]].m_Datas[0].first(1) - raster_eps);
			assert(x < width);
			assert(y < height);
			min_x = std::min(min_x, x);
			min_y = std::min(min_y, y);
			max_x = std::max(max_x, x);
			max_y = std::max(max_y, y);
		}
		for (uint64_t _i = min_x; _i <= max_x; ++_i)
		{
			for (uint64_t _j = min_y; _j <= max_y; ++_j)
			{
				if (state == RasterizationState::Solid)
				{
					if (IsPixelInTriangle(_i, _j, vertices[indices[i]].m_Datas[0].first, vertices[indices[i + 1]].m_Datas[0].first, vertices[indices[i + 2]].m_Datas[0].first))
					{
						//todo
						pic_out.DrawPoint(_i, _j, RGBA(255, 0, 0, 255));
					}
				}
				else if (state == RasterizationState::Wireframe)
				{
					if (IsPixelOnTriangle(_i, _j, vertices[indices[i]].m_Datas[0].first, vertices[indices[i + 1]].m_Datas[0].first, vertices[indices[i + 2]].m_Datas[0].first))
					{
						//todo
						pic_out.DrawPoint(_i, _j, RGBA(255, 0, 0, 255));
					}
				}
			}
		}
	}
}
