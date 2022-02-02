﻿#include <iostream>
#include "Eigen/Dense"
#include "taskflow/taskflow.hpp"
#include "Picture.h"
#include "Camera.h"
#include "ViewPort.h"
#include "InputData.h"
#include "Rasterizer.h"
#include "Output.h"

using namespace std;
using namespace Eigen;
using namespace LearnGames;

void TestInterpalot()
{
	LearnGames::Vector4 p(0.5f, 0.5f, 0, 1);
	LearnGames::Vector4 p0(0, 0, 0, 1);
	LearnGames::Vector4 p1(1, 0, 0, 1);
	LearnGames::Vector4 p2(1, 1, 0, 1);
	LearnGames::Vector2 d0(0, 0);
	LearnGames::Vector2 d1(1, 0);
	LearnGames::Vector2 d2(1, 1);
	auto re = Interpolat<2>(p, p0, p1, p2, d0, d1, d2);
	assert((re(0) - 0.5f) < 0.000001f);
	assert((re(1) - 0.5f) < 0.000001f);
}

void TestRender()
{
	Camera camera;
	ViewPort view_port;
	std::vector<LearnGames::Vertex> vertices(3);
	vertices[0].m_Datas[0].first = LearnGames::Vector4(0.0f, 6.0f, 0.0f, 1.0f);
	vertices[1].m_Datas[0].first = LearnGames::Vector4(6.0f, -3.0f, 0.0f, 1.0f);
	vertices[2].m_Datas[0].first = LearnGames::Vector4(-6.0f, -3.0f, 0.0f, 1.0f);
	std::vector<uint64_t> indices = { 0,1,2 };
	camera.m_WHRatio = 1920.0f / 1080.0f;
	view_port.m_Width = 1920;
	view_port.m_Height = 1080;
	camera.m_Position << 0.0f, 0.0f, -10.0f, 1.0f;
	camera.m_Look << 0.0f, 0.0f, 1.0f, 0.0f;
	camera.m_Up << 0.0f, 1.0f, 0.0f, 0.0f;
	LearnGames::Matrix4 vp_mat = camera.GetViewMatrix() * camera.GetProjectionMatrix();
	for (auto& i : vertices)
	{
		i.m_Datas[0].first = i.m_Datas[0].first * vp_mat;
		i.m_Datas[0].first /= i.m_Datas[0].first(3);
		i.m_Datas[0].first = i.m_Datas[0].first * view_port.GetViewPortMatrix();
	}
	auto solids = Rasterize(1920, 1080, vertices, indices, RasterizationState::Solid, 4);
	auto pic_out = DrawTriangles(1920, 1080, solids, RGBA(0, 0, 0, 255), 4);
	pic_out.Print("test_rr_m2.png");

	auto wireframe = Rasterize(1920, 1080, vertices, indices, RasterizationState::Wireframe, 4);
	auto pic_out2 = DrawTriangles(1920, 1080, wireframe, RGBA(0, 0, 0, 255), 4);
	pic_out2.Print("test2_rr_m2.png");
}

int main()
{
	TestInterpalot();
	TestRender();
	return 0;
}
