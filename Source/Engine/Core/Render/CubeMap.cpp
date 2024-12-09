#include "CubeMap.h"

#include "PathManager.h"
#include "ResourceManager.h"
#include "stb_image.h"

ST::CubeMap::CubeMap(const ST_VECTOR<ST_STRING>& imagePaths) {
	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1,&_cubeMapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP,_cubeMapId);
	for(unsigned int i=0;i<imagePaths.size();++i) {
		int width, height, channel;
		auto image = ResourceManager::GetResourceManager().LoadImageToCharPtr(PathManager::GetFullPath(imagePaths[i]),
			width, height, channel);
		if (image) {
			GLenum format;
			if (channel == 1)
				format = GL_RED;
			else if (channel == 3)
				format = GL_RGB;
			else
				format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0, format, width, height, 0, format,GL_UNSIGNED_BYTE, image);
			ResourceManager::GetResourceManager().UnloadImage(image);
		}
		else {
			ST_ERROR("Load image failed");
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	stbi_set_flip_vertically_on_load(true);
}
