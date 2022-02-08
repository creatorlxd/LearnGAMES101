#pragma once
#include "MathDefinition.h"
#include "Data.h"

namespace LearnGames
{
	/*
	position
	normal
	*/
	struct PositionNormalData :public PositionData
	{
	public:
		PositionNormalData();

		Vector4& GetNormal();
		const Vector4& GetNormal()const;
	};

	struct Geometry
	{
		std::vector<PositionNormalData> m_Vertices;
		std::vector<uint64_t> m_Indices;
	};

	Geometry GenerateCube();
}