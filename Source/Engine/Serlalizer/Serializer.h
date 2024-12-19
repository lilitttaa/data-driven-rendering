#pragma once
#include <fstream>
#include <iostream>
#include <string>

#include "HFX/HFX.h"

namespace HFX
{}

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

	template <>
	BinarySerializer& operator<<(HFX::IndirectString& value);

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
	else
	{
		throw std::runtime_error("Type not supported by BinarySerializer");
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

template <>
BinarySerializer& BinarySerializer::operator<<(HFX::IndirectString& value)
{
	if (action_ == SerializerAction::kWrite)
	{
		stream_.write(reinterpret_cast<char*>(&value.length_), sizeof(size_t));
		stream_.write(value.text_, static_cast<size_t>(value.length_));
	}
	else
	{
		stream_.read(reinterpret_cast<char*>(&value.length_), sizeof(size_t));
		delete[] value.text_;
		value.text_ = new char[value.length_];
		stream_.read(const_cast<char*>(value.text_), static_cast<size_t>(value.length_));
	}
	return *this;
}

void TestSerializer();
