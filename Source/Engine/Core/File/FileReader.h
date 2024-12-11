#pragma once
#include <string>

class FileReader
{
public:
	explicit FileReader(const std::string& inFilePath) : filePath(inFilePath) {}

	std::string Read();

private:
	std::string filePath;
};
