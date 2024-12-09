#include "PathManager.h"

#include <direct.h>

ST::ST_STRING ST::PathManager::GetProjectDir() {
	char buffer[256];
	char* dicPath = _getcwd(buffer, sizeof(buffer));
	char* ret     = dicPath;
	for (; *dicPath != '\0'; ++dicPath) {
		if (*dicPath == '\\')
			*dicPath = '/';
	}
	return ST_STRING(ret) + "/../";
}

ST::ST_STRING ST::PathManager::GetResourcePath() {
	return GetProjectDir() + "Resource/";
}

ST::ST_STRING ST::PathManager::GetGamePath() {
	return {};
}

ST::ST_STRING ST::PathManager::GetEnginePath() {
	return {};
}

ST::ST_STRING ST::PathManager::GetFullPath(const ST_STRING& shortPath) {
	if (shortPath.substr(0, 10) == "/Resource/") {
		return GetResourcePath() + shortPath.substr(10);
	}

	ST_ERROR("Path is not a short path");
	return {};
}
