#include "Geometry.h"

using namespace LearnGames;

namespace
{
	void ProcessedNormal(std::vector<PositionNormalData>& vertices)
	{
		for (auto& i : vertices)
		{
			i.GetNormal() /= i.GetNormal()(3);
			i.GetNormal()(3) = 0.0f;
			i.GetNormal().normalize();
			i.GetNormal()(3) = 1.0f;
		}
	}

	Vector4 GenerateTriangle(std::vector<PositionNormalData>& vertices, std::vector<uint64_t>& indices, uint64_t i0, uint64_t i1, uint64_t i2)
	{
		auto& v0 = vertices[i0];
		auto& v1 = vertices[i1];
		auto& v2 = vertices[i2];
		auto normal_v = GetNormalVector(v0.GetPosition(), v1.GetPosition(), v2.GetPosition());
		v0.GetNormal() += normal_v;
		v1.GetNormal() += normal_v;
		v2.GetNormal() += normal_v;
		indices.emplace_back(i0);
		indices.emplace_back(i1);
		indices.emplace_back(i2);

		return normal_v;
	}

	void GeneratePlane(std::vector<PositionNormalData>& vertices, std::vector<uint64_t>& indices, uint64_t i0, uint64_t i1, uint64_t i2, uint64_t i3)
	{
		auto normal_v1 = GenerateTriangle(vertices, indices, i0, i1, i2);
		auto normal_v2 = GenerateTriangle(vertices, indices, i0, i2, i3);
		auto& v0 = vertices[i0];
		auto& v2 = vertices[i2];
		if (IsEqual<4>(normal_v1, normal_v2))	//special judge
		{
			v0.GetNormal() -= normal_v1;
			v2.GetNormal() -= normal_v2;
		}
	}
}

LearnGames::PositionNormalData::PositionNormalData()
{
	m_Datas.emplace_back(std::make_pair(Vector4::Zero(), DataProperty::Interpolation));	//normal
}

Vector4& LearnGames::PositionNormalData::GetNormal()
{
	return m_Datas[1].first;
}

const Vector4& LearnGames::PositionNormalData::GetNormal() const
{
	return m_Datas[1].first;
}

Geometry LearnGames::GenerateCube()
{
	Geometry re;
	re.m_Vertices.resize(8);
	re.m_Vertices[0].GetPosition() = Vector4(-0.5f, 0.5f, -0.5f, 1.0f);		//left-top-front
	re.m_Vertices[1].GetPosition() = Vector4(0.5f, 0.5f, -0.5f, 1.0f);		//right-top-front
	re.m_Vertices[2].GetPosition() = Vector4(0.5f, -0.5f, -0.5f, 1.0f);		//right-bottom-front
	re.m_Vertices[3].GetPosition() = Vector4(-0.5f, -0.5f, -0.5f, 1.0f);	//left-bottom-front
	re.m_Vertices[4].GetPosition() = Vector4(0.5f, -0.5f, 0.5f, 1.0f);		//right-bottom-back
	re.m_Vertices[5].GetPosition() = Vector4(0.5f, 0.5f, 0.5f, 1.0f);		//right-top-back
	re.m_Vertices[6].GetPosition() = Vector4(-0.5f, 0.5f, 0.5f, 1.0f);		//left-top-back
	re.m_Vertices[7].GetPosition() = Vector4(-0.5f, -0.5f, 0.5f, 1.0f);		//left-bottom-back

	GeneratePlane(re.m_Vertices, re.m_Indices, 0, 1, 2, 3);	//front
	GeneratePlane(re.m_Vertices, re.m_Indices, 6, 5, 1, 0);	//top
	GeneratePlane(re.m_Vertices, re.m_Indices, 1, 5, 4, 2);	//right
	GeneratePlane(re.m_Vertices, re.m_Indices, 3, 2, 4, 7);	//bottom
	GeneratePlane(re.m_Vertices, re.m_Indices, 6, 0, 3, 7);	//left
	GeneratePlane(re.m_Vertices, re.m_Indices, 5, 6, 7, 4);	//back

	ProcessedNormal(re.m_Vertices);
	return re;
}
