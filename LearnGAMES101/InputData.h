#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include "MathDefinition.h"

namespace LearnGames
{
	enum class DataProperty :uint8_t
	{
		DoNothing = 0
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