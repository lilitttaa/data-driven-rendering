#pragma once
#include "Core.h"

namespace ST {
class CubeMap;

class Shader;

class Texture2D;

class Model;

class ResourceManager {
public:
	unsigned char* LoadImageToCharPtr(ST_STRING imagePath, int& width, int& height, int& channel);

	void UnloadImage(unsigned char* data);

	bool LoadFileToStr(ST_STRING filePath, ST_STRING& outStr);

	static ResourceManager& GetResourceManager() {
		static ResourceManager _resourceManager;

		return _resourceManager;
	}

	ST_REF<Texture2D> LoadTexture(const ST_STRING& path);

	ST_REF<Model> LoadModel(const ST_STRING& path);

	ST_REF<Shader> LoadShader(const ST_STRING& vertPath, const ST_STRING& fragPath);

	ST_REF<CubeMap> LoadCubeMap(const ST_VECTOR<ST_STRING>& paths);

	void UnloadTexture(const ST_STRING& path);

	void UnloadModel(const ST_STRING& path);

	void UnloadShader(const ST_STRING& vertPath, const ST_STRING& fragPath);

private:
	ST_MAP<ST_STRING, ST_REF<Texture2D>> _textures;

	ST_MAP<ST_STRING, ST_REF<Model>> _models;

	ST_MAP<ST_STRING, ST_REF<Shader>> _shaders;

	ST_MAP<ST_STRING,ST_REF<CubeMap>> _cubeMaps;

	void Init();

	ResourceManager() {
		Init();
	}

};

}
