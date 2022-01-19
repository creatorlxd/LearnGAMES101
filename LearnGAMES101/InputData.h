#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include "MathDefinition.h"

namespace LearnGames
{
	enum class DataProperty :uint8_t
	{
		DoInterpolation = 0,
		DoNothing = 1
	};

	struct Vertex
	{
		/*
		position
		*/
		Vertex();
		std::vector<std::pair<Vector4, DataProperty>> m_Datas;
	};

	struct InputData
	{
		std::vector<Vertex> m_Vertexs;
		std::vector<uint64_t> m_Indexes;
	};
}