#include <iostream>
#include "Eigen/Dense"
#include "Picture.h"
#include "Camera.h"

using namespace std;
using namespace Eigen;
using namespace LearnGames;

int main(void) {
	Camera camera;
	camera.m_Position << 0.0f, 0.0f, -1.0f, 1.0f;
	camera.m_Look << 0.0f, 0.0f, 1.0f, 0.0f;
	camera.m_Up << 0.0f, 1.0f, 0.0f, 0.0f;
	cout << camera.GetViewMatrix() << endl;
	cout << camera.GetProjectionMatrix() << endl;
	cout << camera.GetViewPortMatrix() << endl;
	return 0;
}
