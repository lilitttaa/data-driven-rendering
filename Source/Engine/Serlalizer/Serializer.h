#pragma once
#include <fstream>
#include <string>

enum class SerializerAction
{
	kWrite,
	kRead
};

class BinarySerializer
{
public:
	BinarySerializer(SerializerAction action, std::string file_path);

	~BinarySerializer();

	template <typename T>
	BinarySerializer& operator<<(T& value);

	BinarySerializer& operator<<(std::string& value);

protected:
	SerializerAction action_;
	std::fstream stream_;
	std::string file_path_;
};
