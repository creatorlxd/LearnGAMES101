#pragma once
#include <cmath>
#include <algorithm>
#include "MathDefinition.h"
#include "Data.h"
#include "Geometry.h"

namespace LearnGames
{
	namespace Phong
	{
		struct Material
		{
			Vector4 m_Ambient;
			Vector4 m_Diffuse;
			Vector4 m_Specular; // Specular.w = SpecularPower
			Vector4 m_Reflect;
			Vector4 m_Emissive;
		};

		enum class LightType : uint32_t
		{
			DirectionLight = 0,
			PointLight = 1,
			SpotLight = 2
		};

		struct Light
		{
			Vector4 m_Ambient;
			Vector4 m_Diffuse;
			Vector4 m_Specular;

			Vector3 m_Position;
			float m_Range;

			Vector3 m_Direction;
			float m_SpotLightOption;

			Vector3 m_LightOption;
			LightType m_Type;
		};

		void ComputeDirectionLight(const Material& material, const Light& light, const Vector3& normal, const Vector3& toeye, Vector4& ambient, Vector4& diffuse, Vector4& spec);
		void ComputePointLight(const Material& material, const Light& light, const Vector3& pos, const Vector3& normal, const Vector3& toeye, Vector4& ambient, Vector4& diffuse, Vector4& spec);
		void ComputeSpotLight(const Material& material, const Light& light, const Vector3& pos, const Vector3& normal, const Vector3& toeye, Vector4& ambient, Vector4& diffuse, Vector4& spec);
		void ComputeCommonLight(const Material& material, const Light& light, const Vector3& pos, const Vector3& normal, const Vector3& toeye, Vector4& ambient, Vector4& diffuse, Vector4& spec);

		Vector4 GetColorByLights(const Material& material, std::vector<Light> lights, const Vector3& eyepos, const Vector3& pos, const Vector3& normal);
		void GetColorByLightsEx(const Material& material, std::vector<Light> lights, const Vector3& eyepos, const Vector3& pos, const Vector3& normal, Vector4& ambient, Vector4& diffuse, Vector4& specular);

		/*
		position
		normal
		world_position
		*/
		struct PhongVSOutputData :public PositionNormalData
		{
			PhongVSOutputData();
			Vector4& GetWorldPosition();
			const Vector4& GetWorldPosition()const;
		};

		PhongVSOutputData PhongVertexShader(const Matrix4& world_mat, const Matrix4& vp_mat, const Matrix4& view_port_mat, const PositionNormalData& data);
		Vector4 PhongPixelShader(const Material& material, const std::vector<Light>& Lights, const Vector4& camera_position, const PhongVSOutputData& data);
	}
}