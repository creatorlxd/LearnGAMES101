#include "Rasterizer.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>

LearnGames::Vector2 LearnGames::GetMSAAPoint(uint64_t msaa_way, uint64_t idx)
{
	assert(idx < msaa_way);
	if (msaa_way == 1)
		return Vector2(0.5f, 0.5f);
	else if (msaa_way == 4)
	{
		if (idx == 0)
			return Vector2(0.375f, 0.125f);
		else if (idx == 1)
			return Vector2(0.875f, 0.375f);
		else if (idx == 2)
			return Vector2(0.125f, 0.625f);
		else if (idx == 3)
			return Vector2(0.625f, 0.875f);
		else
			abort();
	}
	else
	{
		std::cerr << "not support this msaa way" << std::endl;
		abort();
	}
}

bool LearnGames::IsPixelInTriangle(float pxf, float pyf, const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2)
{
	LearnGames::Vector2 e01 = LearnGames::Vector4ToVector2(v1 - v0);
	LearnGames::Vector2 e12 = LearnGames::Vector4ToVector2(v2 - v1);
	LearnGames::Vector2 e20 = LearnGames::Vector4ToVector2(v0 - v2);
	LearnGames::Vector2 e0p = LearnGames::Vector2(pxf - v0(0), pyf - v0(1));
	LearnGames::Vector2 e1p = LearnGames::Vector2(pxf - v1(0), pyf - v1(1));
	LearnGames::Vector2 e2p = LearnGames::Vector2(pxf - v2(0), pyf - v2(1));
	float fbuf = 0.0f;

	if ((fbuf = LearnGames::Cross2(e01, e0p)) < -raster_eps)
		return false;
	if (abs(fbuf) <= raster_eps && e01(1) < 0.0f)
		return false;

	if ((fbuf = LearnGames::Cross2(e12, e1p)) < -raster_eps)
		return false;
	if (abs(fbuf) <= raster_eps && e12(1) < 0.0f)
		return false;

	if ((fbuf = LearnGames::Cross2(e20, e2p)) < -raster_eps)
		return false;
	if (abs(fbuf) <= raster_eps && e20(1) < 0.0f)
		return false;

	return true;
}

bool LearnGames::IsPixelOnTriangle(float pxf, float pyf, const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2)
{
	LearnGames::Vector2 e01 = LearnGames::Vector4ToVector2(v1 - v0);
	LearnGames::Vector2 e12 = LearnGames::Vector4ToVector2(v2 - v1);
	LearnGames::Vector2 e20 = LearnGames::Vector4ToVector2(v0 - v2);
	LearnGames::Vector2 e0p = LearnGames::Vector2(pxf - v0(0), pyf - v0(1));
	LearnGames::Vector2 e1p = LearnGames::Vector2(pxf - v1(0), pyf - v1(1));
	LearnGames::Vector2 e2p = LearnGames::Vector2(pxf - v2(0), pyf - v2(1));

	e01.normalize();
	e12.normalize();
	e20.normalize();

	if (abs(LearnGames::Cross2(e01, e0p)) <= 0.75f)
		return true;
	if (abs(LearnGames::Cross2(e12, e1p)) <= 0.75f)
		return true;
	if (abs(LearnGames::Cross2(e20, e2p)) <= 0.75f)
		return true;

	return false;
}

bool LearnGames::IsBackFace(const LearnGames::Vector4& v0, const LearnGames::Vector4& v1, const LearnGames::Vector4& v2)
{
	LearnGames::Vector2 e01 = LearnGames::Vector4ToVector2(v1 - v0);
	LearnGames::Vector2 e12 = LearnGames::Vector4ToVector2(v2 - v1);

	return LearnGames::Cross2(e01, e12) < 0.0f;
}

float LearnGames::InterpolatZ(const LearnGames::Vector4& p, const LearnGames::Vector4& p0, const LearnGames::Vector4& p1, const LearnGames::Vector4& p2)
{
	return LearnGames::Interpolat<1>(p, p0, p1, p2, LearnGames::Vector<1>(p0(2)), LearnGames::Vector<1>(p1(2)), LearnGames::Vector<1>(p2(2)))(0);
}
