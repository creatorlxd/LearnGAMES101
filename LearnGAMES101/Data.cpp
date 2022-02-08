#include "Data.h"

LearnGames::PositionData::PositionData()
{
	m_Datas.emplace_back(std::make_pair(Vector4::Zero(), DataProperty::Position));		//position
}

LearnGames::Vector4& LearnGames::PositionData::GetPosition()
{
	return m_Datas[0].first;
}

const LearnGames::Vector4& LearnGames::PositionData::GetPosition() const
{
	return m_Datas[0].first;
}
