#include "MathDefinition.h"

using namespace LearnGames;

Vector2 LearnGames::Vector4ToVector2(const Vector4& v)
{
	return Vector2(v(0), v(1));
}

float LearnGames::Cross2(const Vector2& v1, const Vector2& v2)
{
	return v1(0) * v2(1) - v1(1) * v2(0);
}
