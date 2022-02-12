#pragma once
#include <functional>
#include "taskflow/taskflow.hpp"
#include "Rasterizer.h"
#include "Output.h"
#include "Picture.h"

namespace LearnGames
{
	template<typename VSInputDataType, typename VSOutputDataType, typename = std::enable_if_t<std::is_base_of_v<Data, VSInputDataType>&& std::is_base_of_v<Data, VSOutputDataType>, void >>
	inline void Render(ColorDepthBuffer& target, const std::vector<VSInputDataType>& vertices, const std::vector<uint64_t>& indices, const std::function<VSOutputDataType(const VSInputDataType&)>& vs_func, const std::function<Vector4(const VSOutputDataType&)>& ps_func, RasterizationState rs_state = RasterizationState::Solid)
	{
		std::vector<VSOutputDataType> vec_buffer;
		//vs stage
		for (auto& i : vertices)
		{
			vec_buffer.emplace_back(vs_func(i));
		}
		//rs stage
		auto pixels = Rasterize(target.GetWidth(), target.GetHeight(), vec_buffer, indices, rs_state, target.GetMSAA());
		//ps stage
		DrawTriangles(target, pixels, ps_func);
	}

	template<typename VSInputDataType, typename VSOutputDataType, typename = std::enable_if_t<std::is_base_of_v<Data, VSInputDataType>&& std::is_base_of_v<Data, VSOutputDataType>, void >>
	inline tf::Taskflow RenderTaskFlow(ColorDepthBuffer& target, const std::vector<VSInputDataType>& vertices, const std::vector<uint64_t>& indices, const std::function<VSOutputDataType(const VSInputDataType&)>& vs_func, const std::function<Vector4(const VSOutputDataType&)>& ps_func, RasterizationState rs_state = RasterizationState::Solid)
	{
		tf::Taskflow task_flow;
		task_flow.emplace([&, rs_state, vs_func, ps_func](tf::Subflow& sbf) {
			std::vector<VSOutputDataType> vec_buffer(vertices.size());
			//vs stage
			tf::Task vs_task = sbf.for_each_index(std::size_t(0), vertices.size(), std::size_t(1),
				[&](std::size_t idx) {
					vec_buffer[idx] = vs_func(vertices[idx]);
				}
			).name("vs_stage");

			std::vector<std::vector<Pixel>> pixels;
			//rs stage
			tf::Task rs_task = sbf.emplace([&](tf::Subflow& sbf2)
				{
					RasterizeTask(sbf2, pixels, target.GetWidth(), target.GetHeight(), vec_buffer, indices, rs_state, target.GetMSAA());
				}
			).name("rs_stage");
			//ps stage
			tf::Task ps_task = sbf.emplace([&](tf::Subflow& sbf2)
				{
					DrawTrianglesTask(sbf2, target, pixels, ps_func);
				}
			).name("ps_stage");

			vs_task.precede(rs_task);
			rs_task.precede(ps_task);
			sbf.join();
			}
		).name("render");
		return task_flow;
	}

}