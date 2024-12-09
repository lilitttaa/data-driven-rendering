#pragma once

#include"pch.h"
#include"Core.h"
#include"glm.hpp"


namespace ST {
class Material;

class PointLight;

class DirLight;

class Shader {
public:
	Shader(ST_STRING vertShaderPath, ST_STRING fragShaderPath);

	static ST_REF<Shader> CreateShader(ST_STRING vertShaderPath, ST_STRING fragShaderPath);

	inline void UseShader() const {
		glUseProgram(_shaderId);
	}

	inline uint32_t GetShaderId() const {
		return _shaderId;
	}

	void SetInt(ST_STRING propName, int value) const;

	void SetFloat(ST_STRING propName, float value) const;

	void SetMat4(ST_STRING propName, glm::mat4 mat) const;

	void SetVec3(ST_STRING propName, glm::vec3 vec) const;

	void SetVec4(ST_STRING propName, glm::vec4 vec) const;

	void SetDirLight(ST_STRING proName, ST_REF<DirLight> light) const;

	void SetPointLight(ST_STRING proName,ST_REF<PointLight> light) const;

	void SetMaterial(ST_STRING proName,ST_REF<Material> material) const;

protected:
	unsigned int _shaderId;

};
}
