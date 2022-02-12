#include <iostream>
#include <fstream>
#include "Eigen/Dense"
#include "taskflow/taskflow.hpp"
#include "Picture.h"
#include "Camera.h"
#include "ViewPort.h"
#include "Data.h"
#include "Rasterizer.h"
#include "Output.h"
#include "Render.h"
#include "Geometry.h"
#include "Phong.h"

using namespace std;
using namespace LearnGames;

void TestInterpolat()
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

/*
position
color
*/
struct PositionColorData :public PositionData
{
	PositionColorData()
	{
		m_Datas.emplace_back(std::make_pair(LearnGames::Vector4(0, 0, 0, 1), DataProperty::Interpolation));
	}

	LearnGames::Vector4& GetColor()
	{
		return m_Datas[1].first;
	}

	const LearnGames::Vector4& GetColor()const
	{
		return m_Datas[1].first;
	}
};

void TestRender()
{
	Camera camera;
	ViewPort view_port;
	std::vector<PositionColorData> vertices(3);
	vertices[0].GetPosition() = LearnGames::Vector4(0.0f, 6.0f, 0.0f, 1.0f);
	vertices[1].GetPosition() = LearnGames::Vector4(6.0f, -3.0f, 0.0f, 1.0f);
	vertices[2].GetPosition() = LearnGames::Vector4(-6.0f, -3.0f, 0.0f, 1.0f);
	vertices[0].GetColor() = LearnGames::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[1].GetColor() = LearnGames::Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[2].GetColor() = LearnGames::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	std::vector<uint64_t> indices = { 0,1,2 };
	camera.m_WHRatio = 1920.0f / 1080.0f;
	view_port.m_Width = 1920;
	view_port.m_Height = 1080;
	camera.m_Position << 0.0f, 0.0f, -10.0f, 1.0f;
	camera.m_Look << 0.0f, 0.0f, 1.0f, 0.0f;
	camera.m_Up << 0.0f, 1.0f, 0.0f, 0.0f;
	LearnGames::Matrix4 vp_mat = camera.GetViewMatrix() * camera.GetProjectionMatrix();
	LearnGames::Matrix4 viewport_mat = view_port.GetViewPortMatrix();
	auto vs_func = [&vp_mat, &viewport_mat](const PositionColorData& data)->PositionColorData {
		PositionColorData re;
		re.GetPosition() = data.GetPosition() * vp_mat;
		re.GetPosition() /= re.GetPosition()(3);
		re.GetPosition() = re.GetPosition() * viewport_mat;

		re.GetColor() = data.GetColor();

		return re;
	};
	auto ps_func = [](const PositionColorData& data) -> LearnGames::Vector4 {
		return data.GetColor();
	};
	ColorDepthBuffer cdb1(1920, 1080, 4);
	Render<PositionColorData, PositionColorData>(cdb1, vertices, indices, vs_func, ps_func, RasterizationState::Solid);
	cdb1.PrintToPicture().Print("test_color_1.png");
	cdb1.ClearContent();
	Render<PositionColorData, PositionColorData>(cdb1, vertices, indices, vs_func, ps_func, RasterizationState::Wireframe);
	cdb1.PrintToPicture().Print("test_color_2.png");
}

void TestRenderTask()
{
	tf::Executor executor;
	Camera camera;
	ViewPort view_port;
	std::vector<PositionColorData> vertices(3);
	vertices[0].GetPosition() = LearnGames::Vector4(0.0f, 6.0f, 0.0f, 1.0f);
	vertices[1].GetPosition() = LearnGames::Vector4(6.0f, -3.0f, 0.0f, 1.0f);
	vertices[2].GetPosition() = LearnGames::Vector4(-6.0f, -3.0f, 0.0f, 1.0f);
	vertices[0].GetColor() = LearnGames::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[1].GetColor() = LearnGames::Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[2].GetColor() = LearnGames::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	std::vector<uint64_t> indices = { 0,1,2 };
	camera.m_WHRatio = 1920.0f / 1080.0f;
	view_port.m_Width = 1920;
	view_port.m_Height = 1080;
	camera.m_Position << 0.0f, 0.0f, -10.0f, 1.0f;
	camera.m_Look << 0.0f, 0.0f, 1.0f, 0.0f;
	camera.m_Up << 0.0f, 1.0f, 0.0f, 0.0f;
	LearnGames::Matrix4 vp_mat = camera.GetViewMatrix() * camera.GetProjectionMatrix();
	LearnGames::Matrix4 viewport_mat = view_port.GetViewPortMatrix();
	auto vs_func = [&vp_mat, &viewport_mat](const PositionColorData& data)->PositionColorData {
		PositionColorData re;
		re.GetPosition() = data.GetPosition() * vp_mat;
		re.GetPosition() /= re.GetPosition()(3);
		re.GetPosition() = re.GetPosition() * viewport_mat;

		re.GetColor() = data.GetColor();

		return re;
	};
	auto ps_func = [](const PositionColorData& data) -> LearnGames::Vector4 {
		return data.GetColor();
	};
	ColorDepthBuffer cdb1(1920, 1080, 4);
	auto solid_tf = RenderTaskFlow<PositionColorData, PositionColorData>(cdb1, vertices, indices, vs_func, ps_func, RasterizationState::Solid);
	executor.run(solid_tf).wait();
	cdb1.PrintToPicture().Print("test_color_1_tf.png");
	cdb1.ClearContent();
	auto wireframe_tf = RenderTaskFlow<PositionColorData, PositionColorData>(cdb1, vertices, indices, vs_func, ps_func, RasterizationState::Wireframe);
	executor.run(wireframe_tf).wait();
	cdb1.PrintToPicture().Print("test_color_2_tf.png");
}

void TestPhong()
{
	using namespace Phong;
	Camera camera;
	ViewPort view_port;
	Geometry cube = GenerateCube();
	Matrix4 world_mat = GetScalaTransform(3, 3, 3) * GetRotationYTransform(PI / 4.0f) * GetMoveTransform(0, 0, 0);
	camera.m_WHRatio = 1920.0f / 1080.0f;
	view_port.m_Width = 1920;
	view_port.m_Height = 1080;
	camera.m_Position << 0.0f, 0.0f, -5.0f, 1.0f;
	camera.m_Look << 0.0f, 0.0f, 1.0f, 0.0f;
	camera.m_Up << 0.0f, 1.0f, 0.0f, 0.0f;
	LearnGames::Matrix4 vp_mat = camera.GetViewMatrix() * camera.GetProjectionMatrix();
	LearnGames::Matrix4 viewport_mat = view_port.GetViewPortMatrix();
	Material material;
	material.m_Ambient = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	material.m_Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	material.m_Specular = Vector4(1.0f, 1.0f, 1.0f, 8.0f);
	material.m_Emissive = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	Light light;
	light.m_Type = LightType::PointLight;
	light.m_Ambient = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	light.m_Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	light.m_Specular = Vector4(1.0f, 1.0f, 1.0f, 8.0f);
	light.m_Position = Vector3(-5.0f, 0.0f, 0.0f);
	light.m_Direction = Vector3(0.0f, 0.0f, 1.0f);
	light.m_Range = 1000.0f;
	light.m_LightOption = Vector3(0.0f, 0.5f, 0.0f);
	light.m_SpotLightOption = 4.0f;
	std::vector<Light> lights;
	lights.emplace_back(light);
	ColorDepthBuffer cdb(1920, 1080, 4);
	Render<PositionNormalData, PhongVSOutputData>(cdb, cube.m_Vertices, cube.m_Indices,
		std::bind(PhongVertexShader, std::cref(world_mat), std::cref(vp_mat), std::cref(viewport_mat), std::placeholders::_1),
		std::bind(PhongPixelShader, std::cref(material), std::cref(lights), std::cref(camera.m_Position), std::placeholders::_1), RasterizationState::Solid);
	cdb.PrintToPicture().Print("test_phong1.png");
}

int main()
{
	//TestInterpolat();
	//TestRender();
	//TestRenderTask();
	TestPhong();
	return 0;
}
