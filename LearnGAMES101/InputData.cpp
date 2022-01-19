#include "InputData.h"

LearnGames::Vertex::Vertex()
{
	m_Datas.emplace_back(std::make_pair(Vector4(), DataProperty::DoInterpolation));		//position
}
