#include "Phong.h"

using namespace LearnGames;
using namespace LearnGames::Phong;

void LearnGames::Phong::ComputeDirectionLight(const Material& material, const Light& light, const Vector3& normal, const Vector3& toeye, Vector4& ambient, Vector4& diffuse, Vector4& spec)
{
	ambient = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	ambient = material.m_Ambient.cwiseProduct(light.m_Ambient);

	float diffuse_factor = (-1.0f * light.m_Direction).dot(normal);

	if (diffuse_factor > 0.0f)
	{
		Vector3 v = Reflect(light.m_Direction, normal);
		float spec_factor = pow(std::max(v.dot(toeye), 0.0f), material.m_Specular(3));

		diffuse = (diffuse_factor * material.m_Diffuse).cwiseProduct(light.m_Diffuse);
		spec = (spec_factor * material.m_Specular).cwiseProduct(light.m_Specular);
	}
}

void LearnGames::Phong::ComputePointLight(const Material& material, const Light& light, const Vector3& pos, const Vector3& normal, const Vector3& toeye, Vector4& ambient, Vector4& diffuse, Vector4& spec)
{
	ambient = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	Vector3 light_dir = pos - light.m_Position;

	float dis = light_dir.norm();

	if (dis > light.m_Range)
		return;

	ambient = material.m_Ambient.cwiseProduct(light.m_Ambient);
	light_dir.normalize();

	float diffuse_factor = (-1.0f * light_dir).dot(normal);

	if (diffuse_factor > 0.0f)
	{
		Vector3 v = Reflect(light_dir, normal);
		float spec_factor = pow(std::max(v.dot(toeye), 0.0f), material.m_Specular(3));

		diffuse = (diffuse_factor * material.m_Diffuse).cwiseProduct(light.m_Diffuse);
		spec = (spec_factor * material.m_Specular).cwiseProduct(light.m_Specular);
	}

	float att = 1.0f / (light.m_LightOption.dot(Vector3(1.0f, dis, dis * dis)));
	diffuse *= att;
	spec *= att;
}

void LearnGames::Phong::ComputeSpotLight(const Material& material, const Light& light, const Vector3& pos, const Vector3& normal, const Vector3& toeye, Vector4& ambient, Vector4& diffuse, Vector4& spec)
{
	ambient = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	Vector3 light_vec = pos - light.m_Position;

	float dis = light_vec.norm();

	if (dis > light.m_Range)
		return;

	ambient = material.m_Ambient.cwiseProduct(light.m_Ambient);
	light_vec.normalize();

	float diffuse_factor = (-1.0f * light_vec).dot(normal);

	if (diffuse_factor > 0.0f)
	{
		Vector3 v = Reflect(light_vec, normal);
		float spec_factor = pow(std::max(v.dot(toeye), 0.0f), material.m_Specular(3));

		diffuse = (diffuse_factor * material.m_Diffuse).cwiseProduct(light.m_Diffuse);
		spec = (spec_factor * material.m_Specular).cwiseProduct(light.m_Specular);
	}

	float spot = pow(std::max(light_vec.dot(light.m_Direction), 0.0f), light.m_SpotLightOption);

	float att = spot / (light.m_LightOption.dot(Vector3(1.0f, dis, dis * dis)));
	ambient *= att;
	diffuse *= att;
	spec *= att;
}

void LearnGames::Phong::ComputeCommonLight(const Material& material, const Light& light, const Vector3& pos, const Vector3& normal, const Vector3& toeye, Vector4& ambient, Vector4& diffuse, Vector4& spec)
{
	switch (light.m_Type)
	{
	case LightType::DirectionLight:		//Direction Light
	{
		ComputeDirectionLight(material, light, normal, toeye, ambient, diffuse, spec);
		break;
	}
	case LightType::PointLight:		//Point Light
	{
		ComputePointLight(material, light, pos, normal, toeye, ambient, diffuse, spec);
		break;
	}
	case LightType::SpotLight:		//Spot Light
	{
		ComputeSpotLight(material, light, pos, normal, toeye, ambient, diffuse, spec);
		break;
	}
	default:
	{
		ambient = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		diffuse = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		spec = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		break;
	}
	}
}

Vector4 LearnGames::Phong::GetColorByLights(const Material& material, std::vector<Light> lights, const Vector3& eyepos, const Vector3& pos, const Vector3& normal)
{
	Vector4 litcolor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 ambient = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 diffuse = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 specular = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	Vector3 toeye = (eyepos - pos).normalized();

	for (unsigned int i = 0; i < lights.size(); i++)
	{
		ComputeCommonLight(material, lights[i], pos, normal, toeye, ambient, diffuse, specular);
		litcolor += ambient + diffuse + specular;
	}

	litcolor(3) = material.m_Diffuse(3);

	return litcolor;
}

void LearnGames::Phong::GetColorByLightsEx(const Material& material, std::vector<Light> lights, const Vector3& eyepos, const Vector3& pos, const Vector3& normal, Vector4& ambient, Vector4& diffuse, Vector4& specular)
{
	ambient = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	Vector4 A = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 D = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 S = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	Vector3 toeye = (eyepos - pos).normalized();

	for (unsigned int i = 0; i < lights.size(); i++)
	{
		ComputeCommonLight(material, lights[i], pos, normal, toeye, A, D, S);
		ambient += A;
		diffuse += D;
		specular += S;
	}
}

PhongVSOutputData LearnGames::Phong::PhongVertexShader(const Matrix4& world_mat, const Matrix4& vp_mat, const Matrix4& view_port_mat, const PositionNormalData& data)
{
	PhongVSOutputData re;
	re.GetPosition() = data.GetPosition() * world_mat * vp_mat;
	re.GetPosition() /= re.GetPosition()(3);
	re.GetPosition() = re.GetPosition() * view_port_mat;

	re.GetWorldPosition() = data.GetPosition() * world_mat;

	re.GetNormal() = (data.GetNormal() * (world_mat.inverse().transpose()));
	re.GetNormal()(3) = 0.0f;
	re.GetNormal().normalize();
	return re;
}

Vector4 LearnGames::Phong::PhongPixelShader(const Material& material, const std::vector<Light>& Lights, const Vector4& camera_position, const PhongVSOutputData& data)
{
	Vector4 ambient(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 diffuse(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 specular(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 litcolor(0.0f, 0.0f, 0.0f, 0.0f);
	GetColorByLightsEx(material, Lights, Vector4ToVector3(camera_position), Vector4ToVector3(data.GetWorldPosition()), Vector4ToVector3(data.GetNormal()).normalized(), ambient, diffuse, specular);
	litcolor = ambient + diffuse + material.m_Emissive + specular;
	litcolor(3) = material.m_Diffuse(3);
	return litcolor;
}

LearnGames::Phong::PhongVSOutputData::PhongVSOutputData()
{
	m_Datas.emplace_back(std::make_pair(Vector4::Zero(), DataProperty::Interpolation));
}

Vector4& LearnGames::Phong::PhongVSOutputData::GetWorldPosition()
{
	return m_Datas[2].first;
}

const Vector4& LearnGames::Phong::PhongVSOutputData::GetWorldPosition() const
{
	return m_Datas[2].first;
}
