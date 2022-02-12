#pragma once
#include <functional>
#include <mutex>
#include "taskflow/taskflow.hpp"
#include "Rasterizer.h"

namespace LearnGames
{
	struct ColorDepthBuffer
	{
		ColorDepthBuffer(uint64_t width, uint64_t height, uint64_t msaa_way = 1, Vector4 background_color = Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		~ColorDepthBuffer();
		ColorDepthBuffer(const ColorDepthBuffer& c);
		ColorDepthBuffer(ColorDepthBuffer&& c);

		ColorDepthBuffer& operator= (const ColorDepthBuffer& c);
		ColorDepthBuffer& operator=(ColorDepthBuffer&& c);

		Vector4& GetColor(uint64_t x, uint64_t y, uint64_t msaa_idx);
		const Vector4& GetColor(uint64_t x, uint64_t y, uint64_t msaa_idx)const;

		float& GetDepth(uint64_t x, uint64_t y, uint64_t msaa_idx);
		const float& GetDepth(uint64_t x, uint64_t y, uint64_t msaa_idx)const;

		uint64_t GetWidth()const;
		uint64_t GetHeight()const;
		uint64_t GetMSAA()const;

		Picture PrintToPicture()const;

		void ClearContent(Vector4 background_color = Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	private:
		void RawClear();
	private:
		Vector4* m_pColorBuffer;
		float* m_pDepthBuffer;
		uint64_t m_Width;
		uint64_t m_Height;
		uint64_t m_MSAA;
	};

	RGBA Vector4ToRGBA(const Vector4& fcolor);

	template<typename InputDataType, typename = std::enable_if_t<std::is_base_of_v<Data, InputDataType>, void >>
	inline void DrawTriangles(ColorDepthBuffer& target, const std::vector<std::vector<Pixel>>& data, const std::function<Vector4(const InputDataType&)>& ps_func)
	{
		for (auto& triangle : data)
		{
			for (auto& p : triangle)
			{
				Vector4 fcolor = ps_func(*reinterpret_cast<const InputDataType*>(&p.m_Data));
				for (auto& mp : p.m_MSAAPoints)
				{
					if (target.GetDepth(p.m_X, p.m_Y, mp.first) > mp.second)
					{
						target.GetDepth(p.m_X, p.m_Y, mp.first) = mp.second;
						target.GetColor(p.m_X, p.m_Y, mp.first) = fcolor;
					}
				}
			}
		}
	}

	template<typename InputDataType, typename = std::enable_if_t<std::is_base_of_v<Data, InputDataType>, void >>
	inline void DrawTrianglesTask(tf::Subflow& sbf, ColorDepthBuffer& target, const std::vector<std::vector<Pixel>>& data, const std::function<Vector4(const InputDataType&)>& ps_func)
	{
		std::mutex* pmutexs = new std::mutex[target.GetWidth() * target.GetHeight() * target.GetMSAA()];

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
										for (auto& mp : p.m_MSAAPoints)
										{
											std::lock_guard<std::mutex> locker(pmutexs[(p.m_Y * target.GetWidth() + p.m_X) * target.GetMSAA() + mp.first]);
											if (target.GetDepth(p.m_X, p.m_Y, mp.first) > mp.second)
											{
												target.GetDepth(p.m_X, p.m_Y, mp.first) = mp.second;
												target.GetColor(p.m_X, p.m_Y, mp.first) = fcolor;
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

		sbf.join();

		delete[] pmutexs;
	}

}