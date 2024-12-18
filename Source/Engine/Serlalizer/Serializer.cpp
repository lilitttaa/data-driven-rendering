#include "Serializer.h"

BinarySerializer::BinarySerializer(SerializerAction action, std::string file_path):
	action_(action), stream_(), file_path_(std::move(file_path))
{
	if (action_ == SerializerAction::kWrite) { stream_.open(file_path_, std::ios::out | std::ios::binary); }
	else { stream_.open(file_path_, std::ios::in | std::ios::binary); }
	if (!stream_.is_open()) { throw std::runtime_error("Failed to open file: " + file_path_); }
}

BinarySerializer::~BinarySerializer() { stream_.close(); }

template <typename T>
BinarySerializer& BinarySerializer::operator<<(T& value)
{
	if (action_ == SerializerAction::kWrite)
	{
		const char* buffer = reinterpret_cast<char*>(&value);
		stream_.write(buffer, sizeof(T));
	}
	else { stream_.read(reinterpret_cast<char*>(&value), sizeof(T)); }
	return *this;
}

BinarySerializer& BinarySerializer::operator<<(std::string& value)
{
	if (action_ == SerializerAction::kWrite)
	{
		uint32_t size = static_cast<uint32_t>(value.size());
		stream_.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
		stream_.write(value.c_str(), size);
	}
	else
	{
		uint32_t size = 0;
		stream_.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
		value.resize(size);
		stream_.read(&value[0], size);
	}
	return *this;
}
