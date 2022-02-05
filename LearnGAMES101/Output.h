#pragma once
#include <functional>
#include <mutex>
#include "taskflow/taskflow.hpp"
#include "Rasterizer.h"

namespace LearnGames
{
	template<typename InputDataType, typename = std::enable_if_t<std::is_base_of_v<Data, InputDataType>, void >>
	inline Picture DrawTriangles(uint64_t width, uint64_t height, const std::vector<std::vector<Pixel>>& data, RGBA background_color, const std::function<Vector4(const InputDataType&)>& ps_func, uint64_t msaa_way = 1)
	{
		assert(msaa_way > 0);
		Picture re(width, height);

		std::vector<std::vector<float>> zbuffer(height, std::vector<float>(width * msaa_way, 2.0f));
		std::vector<std::vector<RGBA>> color_buf(height, std::vector<RGBA>(width * msaa_way, background_color));

		for (auto& triangle : data)
		{
			for (auto& p : triangle)
			{
				Vector4 fcolor = ps_func(*reinterpret_cast<const InputDataType*>(&p.m_Data));
				RGBA color(0, 0, 0, 0);
				for (uint64_t i = 0; i < 4; ++i)
				{
					int buf = floor(fcolor(i) * 256.0f);
					if (buf > 255)
						buf = 255;
					if (buf < 0)
						buf = 0;
					color.m_Array[i] = buf;
				}
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

	template<typename InputDataType, typename = std::enable_if_t<std::is_base_of_v<Data, InputDataType>, void >>
	inline void DrawTrianglesTask(tf::Subflow& sbf, Picture& re, uint64_t width, uint64_t height, const std::vector<std::vector<Pixel>>& data, RGBA background_color, const std::function<Vector4(const InputDataType&)>& ps_func, uint64_t msaa_way = 1)
	{
		assert(msaa_way > 0);
		re = Picture(width, height);

		std::vector<std::vector<float>> zbuffer(height, std::vector<float>(width * msaa_way, 2.0f));
		std::vector<std::vector<RGBA>> color_buf(height, std::vector<RGBA>(width * msaa_way, background_color));
		//std::vector<std::vector<std::mutex>> mutexs(height, std::vector<std::mutex>(width * msaa_way));
		std::mutex* pmutexs = new std::mutex[height * width * msaa_way];

		tf::Task draw_task = sbf.emplace([&](tf::Subflow& sbf2)
			{
				for (uint64_t i = 0; i < data.size(); ++i)
				{
					sbf2.emplace([&, i](tf::Subflow& sbf3)
						{
							for (uint64_t j = 0; j < data[i].size(); ++j)
							{
								sbf3.emplace([&, i, j]()
									{
										auto& p = data[i][j];
										Vector4 fcolor = ps_func(*reinterpret_cast<const InputDataType*>(&p.m_Data));
										RGBA color(0, 0, 0, 0);
										for (uint64_t i = 0; i < 4; ++i)
										{
											int buf = floor(fcolor(i) * 256.0f);
											if (buf > 255)
												buf = 255;
											if (buf < 0)
												buf = 0;
											color.m_Array[i] = buf;
										}
										for (auto& mp : p.m_MSAAPoints)
										{
											std::lock_guard<std::mutex> locker(pmutexs[p.m_Y * width + p.m_X * msaa_way + mp.first]);
											if (zbuffer[p.m_Y][p.m_X * msaa_way + mp.first] > mp.second)
											{
												zbuffer[p.m_Y][p.m_X * msaa_way + mp.first] = mp.second;
												color_buf[p.m_Y][p.m_X * msaa_way + mp.first] = color;
											}
										}
									}
								).name("ps_draw_pixel");
							}
						}
					).name("ps_draw_triangle");
				}
			}
		).name("ps_draw");

		tf::Task merge_task = sbf.emplace([&](tf::Subflow& sbf2)
			{
				for (uint64_t i = 0; i < width; ++i)
					for (uint64_t j = 0; j < height; ++j)
					{
						sbf2.emplace([&, i, j]()
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
						).name("merge_pixel");
					}
			}
		).name("merge");

		draw_task.precede(merge_task);

		sbf.join();

		delete[] pmutexs;
	}

}