#pragma once
#include <fstream>
#include <iostream>
#include <string>

#include "HFX/HFX.h"

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

	template <>
	BinarySerializer& operator<<(std::string& value);
	
	template <typename U>
	BinarySerializer& operator<<(std::vector<U>& value);

	SerializerAction GetAction() const { return action_; }

protected:
	SerializerAction action_;
	std::fstream stream_;
	std::string file_path_;
};

template <typename T>
BinarySerializer& BinarySerializer::operator<<(T& value)
{
	if constexpr (std::is_enum<T>::value)
	{
		using UnderlyingType = std::underlying_type_t<T>;
		UnderlyingType underlyingValue = static_cast<UnderlyingType>(value);
		if (action_ == SerializerAction::kWrite)
		{
			const char* buffer = reinterpret_cast<char*>(&underlyingValue);
			stream_.write(buffer, sizeof(UnderlyingType));
		}
		else
		{
			stream_.read(reinterpret_cast<char*>(&underlyingValue), sizeof(UnderlyingType));
			value = static_cast<T>(underlyingValue);
		}
	}
	else if constexpr (std::is_trivial<T>::value)
	{
		if (action_ == SerializerAction::kWrite)
		{
			const char* buffer = reinterpret_cast<char*>(&value);
			stream_.write(buffer, sizeof(T));
		}
		else { stream_.read(reinterpret_cast<char*>(&value), sizeof(T)); }
	}
	else { throw std::runtime_error("Type not supported by BinarySerializer"); }
	return *this;
}

template <typename U>
BinarySerializer& BinarySerializer::operator<<(std::vector<U>& value)
{
	if (action_ == SerializerAction::kWrite)
	{
		uint32_t size = static_cast<uint32_t>(value.size());
		stream_.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
		for (auto& element : value) { *this << element; }
	}
	else
	{
		uint32_t size = 0;
		stream_.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
		value.resize(size);
		for (auto& element : value) { *this << element; }
	}
	return *this;
}

template <>
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

void TestSerializer();
