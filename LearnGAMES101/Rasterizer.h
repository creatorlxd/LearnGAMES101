#pragma once
#include <type_traits>
#include <iostream>
#include <mutex>
#include "taskflow/taskflow.hpp"
#include "MathDefinition.h"
#include "Data.h"
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
		Data m_Data;
	};

	Vector2 GetMSAAPoint(uint64_t msaa_way, uint64_t idx);

	inline const float raster_eps = 0.00001f;

	bool IsPixelInTriangle(float pxf, float pyf, const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2);

	bool IsPixelOnTriangle(float pxf, float pyf, const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2);

	bool IsBackFace(const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2);

	float InterpolatZ(const LearnGames::Vector4& p, const LearnGames::Vector4& p0, const LearnGames::Vector4& p1, const LearnGames::Vector4& p2);

	template<typename InputDataType, typename = std::enable_if_t<std::is_base_of_v<Data, InputDataType>, void >>
	inline std::vector<std::vector<Pixel>> Rasterize(uint64_t width, uint64_t height, const std::vector<InputDataType>& vertices, const std::vector<uint64_t>& indices, RasterizationState state, uint64_t msaa_way = 1)
	{
		assert(indices.size() % 3 == 0);
		assert(msaa_way > 0);
		if (vertices.size() == 0 || indices.size() == 0)
			return std::vector<std::vector<Pixel>>(0);

		uint64_t pos_idx = vertices[0].m_Datas.size();
		//find position tag position
		for (uint64_t i = 0; i < vertices[0].m_Datas.size(); ++i)
		{
			if (vertices[0].m_Datas[i].second == DataProperty::Position)
			{
				pos_idx = i;
				break;
			}
		}
		assert(pos_idx != vertices[0].m_Datas.size());

		std::vector<std::vector<LearnGames::Pixel>> re;

		for (uint64_t i = 0; i < indices.size(); i += 3)
		{
			std::vector<Pixel> triangle;
			//back face culling
			if (IsBackFace(vertices[indices[i]].m_Datas[pos_idx].first, vertices[indices[i + 1]].m_Datas[pos_idx].first, vertices[indices[i + 2]].m_Datas[pos_idx].first))
				continue;

			//calculate aabb box
			int64_t min_x = width, min_y = height;
			int64_t max_x = 0, max_y = 0;
			for (uint64_t j = i; j < i + 3; ++j)
			{
				int64_t x = floor(vertices[indices[j]].m_Datas[pos_idx].first(0));
				int64_t y = floor(vertices[indices[j]].m_Datas[pos_idx].first(1));
				min_x = std::min(min_x, x);
				min_y = std::min(min_y, y);
				max_x = std::max(max_x, x);
				max_y = std::max(max_y, y);
			}
			for (int64_t _i = min_x; _i <= max_x; ++_i)
			{
				for (int64_t _j = min_y; _j <= max_y; ++_j)
				{
					if (_i < 0 || _i >= width || _j < 0 || _j >= height)
						continue;
					bool(*pjudge)(float, float, const Vector4&, const Vector4&, const Vector4&) = nullptr;
					if (state == RasterizationState::Solid)
						pjudge = &IsPixelInTriangle;
					else if (state == RasterizationState::Wireframe)
						pjudge = &IsPixelOnTriangle;

					Pixel pixel;
					Vector4 pixel_point = Vector4::Zero();
					uint64_t cnt = 0;
					float z_total = 0.0f;
					for (uint64_t msaa_idx = 0; msaa_idx < msaa_way; ++msaa_idx)
					{
						Vector2 p = GetMSAAPoint(msaa_way, msaa_idx);
						float pxf = p(0) + (float)_i;
						float pyf = p(1) + (float)_j;
						if (pjudge(pxf, pyf, vertices[indices[i]].m_Datas[pos_idx].first, vertices[indices[i + 1]].m_Datas[pos_idx].first, vertices[indices[i + 2]].m_Datas[pos_idx].first))
						{
							float z = InterpolatZ(Vector4(pxf, pyf, 0, 1), vertices[indices[i]].m_Datas[pos_idx].first, vertices[indices[i + 1]].m_Datas[pos_idx].first, vertices[indices[i + 2]].m_Datas[pos_idx].first);
							z_total += z;
							pixel_point += Vector4(pxf, pyf, z, 1);
							++cnt;
							pixel.m_MSAAPoints.emplace_back(std::make_pair(msaa_idx, z));
						}
					}
					if (z_total < -1.0f * raster_eps)
						continue;
					if (cnt)
					{
						pixel_point /= (float)cnt;
						pixel.m_X = _i;
						pixel.m_Y = _j;
						auto interpolat_ij = HalfInterpolat(pixel_point, vertices[indices[i]].m_Datas[pos_idx].first, vertices[indices[i + 1]].m_Datas[pos_idx].first, vertices[indices[i + 2]].m_Datas[pos_idx].first);
						for (uint64_t d_idx = 0; d_idx < vertices[indices[i]].m_Datas.size(); ++d_idx)
						{
							auto& d0 = vertices[indices[i]].m_Datas[d_idx];
							switch (d0.second)
							{
							case DataProperty::None:
							{
								pixel.m_Data.m_Datas.emplace_back(d0);
								break;
							}

							case DataProperty::Position:
							{
								pixel.m_Data.m_Datas.emplace_back(std::make_pair(pixel_point, d0.second));
								break;
							}

							case DataProperty::Interpolation:
							{
								auto& d1 = vertices[indices[i + 1]].m_Datas[d_idx];
								auto& d2 = vertices[indices[i + 2]].m_Datas[d_idx];
								Vector4 de01 = d1.first - d0.first;
								Vector4 de12 = d2.first - d1.first;
								pixel.m_Data.m_Datas.emplace_back(std::make_pair(interpolat_ij.first * de01 + interpolat_ij.second * de12 + d0.first, d0.second));
								break;
							}

							default:
							{
								std::cerr << "error data property" << std::endl;
								abort();
								break;
							}
							}
						}
						triangle.emplace_back(std::move(pixel));
					}
				}
			}
			if (triangle.size())
				re.emplace_back(std::move(triangle));
		}
		return re;
	}

	template<typename InputDataType, typename = std::enable_if_t<std::is_base_of_v<Data, InputDataType>, void >>
	inline void RasterizeTask(tf::Subflow& sbf, std::vector<std::vector<Pixel>>& re, uint64_t width, uint64_t height, const std::vector<InputDataType>& vertices, const std::vector<uint64_t>& indices, RasterizationState state, uint64_t msaa_way = 1)
	{
		assert(indices.size() % 3 == 0);
		assert(msaa_way > 0);
		std::mutex mutex;	//mutex for re
		re.clear();
		if (vertices.size() == 0 || indices.size() == 0)
			return;

		uint64_t pos_idx = vertices[0].m_Datas.size();
		//find position tag position
		for (uint64_t i = 0; i < vertices[0].m_Datas.size(); ++i)
		{
			if (vertices[0].m_Datas[i].second == DataProperty::Position)
			{
				pos_idx = i;
				break;
			}
		}
		assert(pos_idx != vertices[0].m_Datas.size());

		for (uint64_t i = 0; i < indices.size(); i += 3)
		{
			sbf.emplace([&, i](tf::Subflow& sbf2) {
				std::vector<Pixel> triangle;
				std::mutex triangle_mutex;
				//back face culling
				if (IsBackFace(vertices[indices[i]].m_Datas[pos_idx].first, vertices[indices[i + 1]].m_Datas[pos_idx].first, vertices[indices[i + 2]].m_Datas[pos_idx].first))
					return;

				//calculate aabb box
				int64_t min_x = width, min_y = height;
				int64_t max_x = 0, max_y = 0;
				for (uint64_t j = i; j < i + 3; ++j)
				{
					int64_t x = floor(vertices[indices[j]].m_Datas[pos_idx].first(0));
					int64_t y = floor(vertices[indices[j]].m_Datas[pos_idx].first(1));
					min_x = std::min(min_x, x);
					min_y = std::min(min_y, y);
					max_x = std::max(max_x, x);
					max_y = std::max(max_y, y);
				}
				for (int64_t _i = min_x; _i <= max_x; ++_i)
				{
					for (int64_t _j = min_y; _j <= max_y; ++_j)
					{
						if (_i < 0 || _i >= width || _j < 0 || _j >= height)
							continue;
						sbf2.emplace([&, i, _i, _j]() {
							bool(*pjudge)(float, float, const Vector4&, const Vector4&, const Vector4&) = nullptr;
							if (state == RasterizationState::Solid)
								pjudge = &IsPixelInTriangle;
							else if (state == RasterizationState::Wireframe)
								pjudge = &IsPixelOnTriangle;

							Pixel pixel;
							Vector4 pixel_point = Vector4::Zero();
							uint64_t cnt = 0;
							for (uint64_t msaa_idx = 0; msaa_idx < msaa_way; ++msaa_idx)
							{
								Vector2 p = GetMSAAPoint(msaa_way, msaa_idx);
								float pxf = p(0) + (float)_i;
								float pyf = p(1) + (float)_j;
								if (pjudge(pxf, pyf, vertices[indices[i]].m_Datas[pos_idx].first, vertices[indices[i + 1]].m_Datas[pos_idx].first, vertices[indices[i + 2]].m_Datas[pos_idx].first))
								{
									float z = InterpolatZ(Vector4(pxf, pyf, 0, 1), vertices[indices[i]].m_Datas[pos_idx].first, vertices[indices[i + 1]].m_Datas[pos_idx].first, vertices[indices[i + 2]].m_Datas[pos_idx].first);
									pixel_point += Vector4(pxf, pyf, z, 1);
									++cnt;
									pixel.m_MSAAPoints.emplace_back(std::make_pair(msaa_idx, z));
								}
							}
							if (cnt)
							{
								pixel_point /= (float)cnt;
								pixel.m_X = _i;
								pixel.m_Y = _j;
								auto interpolat_ij = HalfInterpolat(pixel_point, vertices[indices[i]].m_Datas[pos_idx].first, vertices[indices[i + 1]].m_Datas[pos_idx].first, vertices[indices[i + 2]].m_Datas[pos_idx].first);
								for (uint64_t d_idx = 0; d_idx < vertices[indices[i]].m_Datas.size(); ++d_idx)
								{
									auto& d0 = vertices[indices[i]].m_Datas[d_idx];
									switch (d0.second)
									{
									case DataProperty::None:
									{
										pixel.m_Data.m_Datas.emplace_back(d0);
										break;
									}

									case DataProperty::Position:
									{
										pixel.m_Data.m_Datas.emplace_back(std::make_pair(pixel_point, d0.second));
										break;
									}

									case DataProperty::Interpolation:
									{
										auto& d1 = vertices[indices[i + 1]].m_Datas[d_idx];
										auto& d2 = vertices[indices[i + 2]].m_Datas[d_idx];
										Vector4 de01 = d1.first - d0.first;
										Vector4 de12 = d2.first - d1.first;
										pixel.m_Data.m_Datas.emplace_back(std::make_pair(interpolat_ij.first * de01 + interpolat_ij.second * de12 + d0.first, d0.second));
										break;
									}

									default:
									{
										std::cerr << "error data property" << std::endl;
										abort();
										break;
									}
									}
								}
								std::lock_guard<std::mutex> locker(triangle_mutex);
								triangle.emplace_back(std::move(pixel));
							}
							}
						).name("rasterize_judge_pixel");
					}
				}
				sbf2.join();
				if (triangle.size())
				{
					std::lock_guard<std::mutex> locker(mutex);
					re.emplace_back(std::move(triangle));
				}
				}
			).name("rasterize_triangle");
		}

		sbf.join();
	}

}