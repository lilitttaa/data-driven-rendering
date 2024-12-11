#pragma once

#include <direct.h>

#include "Core.h"

namespace ST
{
class PathManager
{
public:
	static ST_STRING GetProjectDir();

	static ST_STRING GetShaderDir();

	static ST_STRING GetHFXDir();

	static ST_STRING GetHFXGeneratedDir();

	static ST_STRING GetResourcePath();

	static ST_STRING GetGamePath();

	static ST_STRING GetEnginePath();

	static ST_STRING GetFullPath(const ST_STRING& shortPath);
};
}
