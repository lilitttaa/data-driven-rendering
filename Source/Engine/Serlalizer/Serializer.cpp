#include "Serializer.h"

BinarySerializer::BinarySerializer(SerializerAction action, std::string file_path):
	action_(action), stream_(), file_path_(std::move(file_path))
{
	if (action_ == SerializerAction::kWrite) { stream_.open(file_path_, std::ios::out | std::ios::binary); }
	else { stream_.open(file_path_, std::ios::in | std::ios::binary); }
	if (!stream_.is_open()) { throw std::runtime_error("Failed to open file: " + file_path_); }
}

BinarySerializer::~BinarySerializer() { stream_.close(); }

#define EXPECT_EQ(a, b) if ((a) != (b)) { std::cout << "Expected: " << a << " Got: " << b << std::endl; }

void TestSerializer()
{
	std::cout << "Starting Serializer Test" << std::endl;
	int wi = 123;
	float wf = 3.14f;
	std::string ws = "Hello, World!";
	HFX::IndirectString wis;
	wis.text_ = "Hello, World!";
	wis.length_ = 13;
	{
		BinarySerializer serializer(SerializerAction::kWrite, "test.bin");
		serializer << wi;
		serializer << wf;
		serializer << ws;
		serializer << wis;
	}


	int ri = 0;
	float rf = 0.0f;
	std::string rs;
	HFX::IndirectString ris;
	{
		BinarySerializer serializer(SerializerAction::kRead, "test.bin");
		serializer << ri;
		serializer << rf;
		serializer << rs;
		serializer << ris;
	}

	EXPECT_EQ(wi, ri)
	EXPECT_EQ(wf, rf)
	EXPECT_EQ(ws, rs)
	EXPECT_EQ(HFX::IndirectString::Equals(wis, ris), true)
	std::cout << "Serializer Test Finished" << std::endl;
}
