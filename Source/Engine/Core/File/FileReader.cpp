#include "FileReader.h"

#include <fstream>
#include <iterator>
#include <stdexcept>

std::string FileReader::Read()
{
	std::ifstream fileStream(filePath);
	if (!fileStream.is_open()) { throw std::runtime_error("Failed to open file: " + filePath); }

	std::string content((std::istreambuf_iterator<char>(fileStream)),
		std::istreambuf_iterator<char>());
	fileStream.close();
	return content;
}
