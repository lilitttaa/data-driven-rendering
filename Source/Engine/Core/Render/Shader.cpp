#include"Shader.h"

#include "Light.h"
#include "Material.h"
#include "PathManager.h"
#include "gtc/type_ptr.hpp"
#include "Resource/ResourceManager.h"

namespace ST {
Shader::Shader(ST_STRING vertShaderPath, ST_STRING fragShaderPath) {
	ST_STRING vertSource;
	ResourceManager::GetResourceManager().LoadFileToStr(PathManager::GetFullPath(vertShaderPath), vertSource);
	const char* vertShaderSource = vertSource.c_str();
	unsigned int vertShader      = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertShader, 1, &vertShaderSource,NULL);
	glCompileShader(vertShader);
	int success;
	glGetShaderiv(vertShader,GL_COMPILE_STATUS, &success);
	if (!success) {
		char info[512];
		glGetShaderInfoLog(vertShader, 512,NULL, info);
		ST_ERROR("Vert Shader Compiler Failed! ::%s\n",info);
		return;
	}

	ST_STRING fragSource;
	ResourceManager::GetResourceManager().LoadFileToStr(PathManager::GetFullPath(fragShaderPath), fragSource);
	const char* fragShaderSource = fragSource.c_str();
	unsigned int fragShader      = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragShaderSource,NULL);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader,GL_COMPILE_STATUS, &success);
	if (!success) {
		char info[512];
		glGetShaderInfoLog(fragShader, 512,NULL, info);
		ST_ERROR("Frag Shader Compiler Failed! ::%s\n",info);
		return;
	}

	_shaderId = glCreateProgram();
	glAttachShader(_shaderId, vertShader);
	glAttachShader(_shaderId, fragShader);
	glLinkProgram(_shaderId);
	glGetProgramiv(_shaderId,GL_COMPILE_STATUS, &success);
	if (!success) {
		char info[512];
		glGetProgramInfoLog(_shaderId, 512,NULL, info);
		std::cout << "Program Link Failed! ::" << info;
		return;
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

ST_REF<Shader> Shader::CreateShader(ST_STRING vertShaderPath, ST_STRING fragShaderPath) {
	return ST_MAKE_REF<Shader>(vertShaderPath, fragShaderPath);
}

void Shader::SetInt(ST_STRING propName, int value) const {
	glUniform1i(glGetUniformLocation(GetShaderId(), propName.c_str()), value);
}

void Shader::SetFloat(ST_STRING propName, float value) const {
	glUniform1f(glGetUniformLocation(GetShaderId(), propName.c_str()), value);
}

void Shader::SetMat4(ST_STRING propName, glm::mat4 mat) const {
	glUniformMatrix4fv(glGetUniformLocation(GetShaderId(), propName.c_str()), 1,GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetVec3(ST_STRING propName, glm::vec3 vec) const {
	glUniform3fv(glGetUniformLocation(GetShaderId(), propName.c_str()), 1, glm::value_ptr(vec));
}

void Shader::SetVec4(ST_STRING propName, glm::vec4 vec) const {
	glUniform4fv(glGetUniformLocation(GetShaderId(), propName.c_str()), 1, glm::value_ptr(vec));
}

void Shader::SetDirLight(ST_STRING proName, ST_REF<DirLight> light) const {
	SetVec3(proName + ".f_Dir", light->_dir);
	SetVec3(proName + ".f_Ia", light->_ia);
	SetVec3(proName + ".f_Id", light->_id);
	SetVec3(proName + ".f_Is", light->_is);
}

void Shader::SetPointLight(ST_STRING proName, ST_REF<PointLight> light) const {
	SetVec3(proName + ".f_LightPos", light->_pos);
	SetVec3(proName + ".f_Ia", light->_ia);
	SetVec3(proName + ".f_Id", light->_id);
	SetVec3(proName + ".f_Is", light->_is);
	SetFloat(proName + ".f_Const", light->_const);
	SetFloat(proName + ".f_Linear", light->_linear);
	SetFloat(proName + ".f_Quadratic", light->_quadratic);
}

void Shader::SetMaterial(ST_STRING proName, ST_REF<Material> material) const {
	int idx = material->_idx * 3;
	SetInt(proName + ".f_Ka", idx);
	SetInt(proName + ".f_Kd", idx + 1);
	SetInt(proName + ".f_Ks", idx + 2);
	SetFloat(proName + ".f_Shinness", material->_shinness);
}
}
