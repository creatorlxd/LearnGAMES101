#pragma once
#include <functional>
#include "Rasterizer.h"
#include "Output.h"
#include "Picture.h"

namespace LearnGames
{
	template<typename VSInputDataType, typename VSOutputDataType, typename = std::enable_if_t<std::is_base_of_v<Data, VSInputDataType>&& std::is_base_of_v<Data, VSOutputDataType>, void >>
	inline Picture Render(uint64_t width, uint64_t height, const std::vector<VSInputDataType>& vertices, const std::vector<uint64_t>& indices, const std::function<VSOutputDataType(const VSInputDataType&)>& vs_func, const std::function<Vector4(const VSOutputDataType&)>& ps_func, RasterizationState rs_state = RasterizationState::Solid, uint64_t msaa_way = 1, RGBA background_color = RGBA(0, 0, 0, 255))
	{
		std::vector<VSOutputDataType> vec_buffer;
		//vs stage
		for (auto& i : vertices)
		{
			vec_buffer.emplace_back(vs_func(i));
		}
		//rs stage
		auto pixels = Rasterize(width, height, vec_buffer, indices, rs_state, msaa_way);
		//ps stage
		return DrawTriangles(width, height, pixels, background_color, ps_func, msaa_way);
	}
}