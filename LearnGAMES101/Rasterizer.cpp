#include "Rasterizer.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace
{
	const float raster_eps = 0.00001f;

	bool IsPixelInTriangle(float pxf, float pyf, const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2)
	{
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

	bool IsPixelOnTriangle(float pxf, float pyf, const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2)
	{
		LearnGames::Vector2 e01 = LearnGames::Vector4ToVector2(v1 - v0);
		LearnGames::Vector2 e12 = LearnGames::Vector4ToVector2(v2 - v1);
		LearnGames::Vector2 e20 = LearnGames::Vector4ToVector2(v0 - v2);
		LearnGames::Vector2 e0p = LearnGames::Vector2(pxf - v0(0), pyf - v0(1));
		LearnGames::Vector2 e1p = LearnGames::Vector2(pxf - v1(0), pyf - v1(1));
		LearnGames::Vector2 e2p = LearnGames::Vector2(pxf - v2(0), pyf - v2(1));

		e01.normalize();
		e12.normalize();
		e20.normalize();

		if (abs(LearnGames::Cross2(e01, e0p)) <= 0.75f)
			return true;
		if (abs(LearnGames::Cross2(e12, e1p)) <= 0.75f)
			return true;
		if (abs(LearnGames::Cross2(e20, e2p)) <= 0.75f)
			return true;

		return false;
	}


	bool IsBackFace(const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2)
	{
		LearnGames::Vector2 e01 = LearnGames::Vector4ToVector2(v1 - v0);
		LearnGames::Vector2 e12 = LearnGames::Vector4ToVector2(v2 - v1);

		return LearnGames::Cross2(e01, e12) < 0.0f;
	}

	float InterpolatZ(const LearnGames::Vector4& p, const LearnGames::Vector4& p0, const LearnGames::Vector4& p1, const LearnGames::Vector4& p2)
	{
		return LearnGames::Interpolat<1>(p, p0, p1, p2, LearnGames::Vector<1>(p0(2)), LearnGames::Vector<1>(p1(2)), LearnGames::Vector<1>(p2(2)))(0);
	}
}

LearnGames::Vector2 LearnGames::GetMSAAPoint(uint64_t msaa_way, uint64_t idx)
{
	assert(idx < msaa_way);
	if (msaa_way == 1)
		return Vector2(0.5f, 0.5f);
	else if (msaa_way == 4)
	{
		if (idx == 0)
			return Vector2(0.375f, 0.125f);
		else if (idx == 1)
			return Vector2(0.875f, 0.375f);
		else if (idx == 2)
			return Vector2(0.125f, 0.625f);
		else if (idx == 3)
			return Vector2(0.625f, 0.875f);
		else
			abort();
	}
	else
	{
		std::cerr << "not support this msaa way" << std::endl;
		abort();
	}
}

std::vector<std::vector<LearnGames::Pixel>> LearnGames::Rasterize(uint64_t width, uint64_t height, const std::vector<Vertex>& vertices, const std::vector<uint64_t>& indices, RasterizationState state, uint64_t msaa_way)
{
	assert(indices.size() % 3 == 0);
	assert(msaa_way > 0);
	std::vector<std::vector<LearnGames::Pixel>> re;

	for (uint64_t i = 0; i < indices.size(); i += 3)
	{
		std::vector<Pixel> triangle;
		//back face culling
		if (IsBackFace(vertices[indices[i]].m_Datas[0].first, vertices[indices[i + 1]].m_Datas[0].first, vertices[indices[i + 2]].m_Datas[0].first))
			continue;

		//calculate aabb box
		uint64_t min_x = width, min_y = height;
		uint64_t max_x = 0, max_y = 0;
		for (uint64_t j = i; j < i + 3; ++j)
		{
			uint64_t x = floor(vertices[indices[j]].m_Datas[0].first(0));
			uint64_t y = floor(vertices[indices[j]].m_Datas[0].first(1));
			//edge process
			if (x == width)
				x -= 1;
			if (y == height)
				y -= 1;
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
				bool(*pjudge)(float, float, const Vector4&, const Vector4&, const Vector4&) = nullptr;
				if (state == RasterizationState::Solid)
					pjudge = &IsPixelInTriangle;
				else if (state == RasterizationState::Wireframe)
					pjudge = &IsPixelOnTriangle;

				Pixel pixel;
				for (uint64_t msaa_idx = 0; msaa_idx < msaa_way; ++msaa_idx)
				{
					Vector2 p = GetMSAAPoint(msaa_way, msaa_idx);
					float pxf = p(0) + (float)_i;
					float pyf = p(1) + (float)_j;
					if (pjudge(pxf, pyf, vertices[indices[i]].m_Datas[0].first, vertices[indices[i + 1]].m_Datas[0].first, vertices[indices[i + 2]].m_Datas[0].first))
					{
						float z = InterpolatZ(Vector4(pxf, pyf, 0, 1), vertices[indices[i]].m_Datas[0].first, vertices[indices[i + 1]].m_Datas[0].first, vertices[indices[i + 2]].m_Datas[0].first);
						pixel.m_MSAAPoints.emplace_back(std::make_pair(msaa_idx, z));
					}
				}
				pixel.m_X = _i;
				pixel.m_Y = _j;
				triangle.emplace_back(std::move(pixel));
			}
		}
		if (triangle.size())
			re.emplace_back(std::move(triangle));
	}
	return re;
}
