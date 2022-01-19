#include <iostream>
#include "Eigen/Dense"
#include "Picture.h"
#include "Camera.h"
#include "ViewPort.h"
#include "InputData.h"
#include "Rasterizer.h"

using namespace std;
using namespace Eigen;
using namespace LearnGames;

int main()
{
	Camera camera;
	ViewPort view_port;
	std::vector<LearnGames::Vertex> vertices(3);
	vertices[0].m_Datas[0].first = LearnGames::Vector4(0.0f, 6.0f, 0.0f, 1.0f);
	vertices[1].m_Datas[0].first = LearnGames::Vector4(6.0f, -3.0f, 0.0f, 1.0f);
	vertices[2].m_Datas[0].first = LearnGames::Vector4(-6.0f, -3.0f, 0.0f, 1.0f);
	std::vector<uint64_t> indices = { 0,1,2 };
	Picture pic_out(1920, 1080);
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
	Rasterize(pic_out, vertices, indices, RasterizationState::Solid);
	pic_out.Print("test.png");
	return 0;
}
