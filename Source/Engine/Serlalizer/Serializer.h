#pragma once
#include <fstream>
#include <iostream>
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

	template <typename T, typename std::enable_if<std::is_trivial<T>::value, bool>::type = true>
	BinarySerializer& operator<<(T& value);

	BinarySerializer& operator<<(std::string& value);

	template<typename T, typename std::enable_if<!std::is_trivial<T>::value, bool>::type = true>
	BinarySerializer& operator<<(T& value);

protected:
	SerializerAction action_;
	std::fstream stream_;
	std::string file_path_;
};

void TestSerializer();
