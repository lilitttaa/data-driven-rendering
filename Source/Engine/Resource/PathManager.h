#pragma once

#include "Core.h"

namespace ST {
class PathManager {
public:
	static ST_STRING GetProjectDir();

	static ST_STRING GetResourcePath();

	static ST_STRING GetGamePath();

	static ST_STRING GetEnginePath();

	static ST_STRING GetFullPath(const ST_STRING& shortPath);
};
}
