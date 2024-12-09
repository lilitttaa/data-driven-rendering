#include "ResourceManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Render/CubeMap.h"
#include "Render/Model.h"
#include "Render/Shader.h"
#include "Render/Texture2D.h"

namespace ST {
void ResourceManager::Init() {
	stbi_set_flip_vertically_on_load(true);
}

unsigned char* ResourceManager::LoadImageToCharPtr(std::string imagePath, int& width, int& height, int& channel) {
	unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &channel, 0);
	if (!data) {
		ST_LOG("Load image failed! %s\n", imagePath.c_str());
		return nullptr;
	}
	return data;
}

void ResourceManager::UnloadImage(unsigned char* data) {
	stbi_image_free(data);
}

bool ResourceManager::LoadFileToStr(std::string filePath, std::string& outStr) {
	std::ifstream fileStream(filePath, std::ios::ate);
	if (!fileStream.is_open()) {
		std::cout << "File Open Failed !" << filePath << std::endl;
		return false;
	}
	int endPos = fileStream.tellg();
	outStr.resize(endPos);
	fileStream.seekg(0, std::ios::beg);
	fileStream.read(&outStr[0], endPos);
	fileStream.close();
	return true;
}

ST_REF<Texture2D> ResourceManager::LoadTexture(const ST_STRING& path) {
	auto it = _textures.find(path);
	if (it != _textures.end()) {
		return it->second;
	}
	auto texture = ST_MAKE_REF<Texture2D>(path);
	_textures.emplace(path, texture);
	return texture;
}

ST_REF<Model> ResourceManager::LoadModel(const ST_STRING& path) {
	auto it = _models.find(path);
	if (it != _models.end()) {
		return it->second;
	}
	auto model = ST_MAKE_REF<Model>(path);
	_models.emplace(path,model);
	return model;
}

ST_REF<Shader> ResourceManager::LoadShader(const ST_STRING& vertPath, const ST_STRING& fragPath) {
	auto it = _shaders.find(vertPath);
	if (it != _shaders.end()) {
		return it->second;
	}
	auto shader = ST_MAKE_REF<Shader>(vertPath,fragPath);
	_shaders.emplace(vertPath,shader);
	return shader;
}


void ResourceManager::UnloadTexture(const ST_STRING& path) {
	_textures.erase(path);
}

void ResourceManager::UnloadModel(const ST_STRING& path) {
	_models.erase(path);
}

void ResourceManager::UnloadShader(const ST_STRING& vertPath, const ST_STRING& fragPath) {
	_shaders.erase(vertPath);
}

ST_REF<CubeMap> ResourceManager::LoadCubeMap(const ST_VECTOR<ST_STRING>& paths) {
	auto it = _cubeMaps.find(paths[0]);
	if (it != _cubeMaps.end()) {
		return it->second;
	}
	auto cubeMap = ST_MAKE_REF<CubeMap>(paths);
	_cubeMaps.emplace(paths[0],cubeMap);
	return cubeMap;
}
}
