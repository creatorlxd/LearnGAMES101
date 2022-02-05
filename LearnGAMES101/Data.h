#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include "MathDefinition.h"

namespace LearnGames
{
	enum class DataProperty :uint8_t
	{
		None = 0,
		Interpolation = 1,
		Position = 2		//changed after rasterization
	};

	struct Data
	{
		std::vector<std::pair<Vector4, DataProperty>> m_Datas;
	};

	/*
	position
	*/
	struct PositionData :public Data
	{
		PositionData();
		Vector4& GetPosition();
		const Vector4& GetPosition()const;
	};

}