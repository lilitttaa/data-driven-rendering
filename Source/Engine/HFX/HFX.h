#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Graphics/Graphics.h"
#include "Serlalizer/Serializer.h"

namespace HFX
{
class IndirectString
{
public:
	IndirectString(): length_(0), text_(nullptr) {}
	size_t length_;
	char const* text_;

	static bool Equals(const IndirectString& a, const IndirectString& b);

	static void Copy(const IndirectString& a, char* buffer, size_t buffer_size)
	{
		const size_t copy_size = a.length_ < buffer_size
		                         ? a.length_
		                         : buffer_size;
		for (size_t i = 0; i < copy_size; ++i) { buffer[i] = a.text_[i]; }
	}

	friend std::ostream& operator<<(std::ostream& os, const IndirectString& str);

	std::string ToString() const
	{
		std::string str;
		for (size_t i = 0; i < length_; ++i) { str += text_[i]; }
		return str;
	}

	friend std::string to_string(const IndirectString& obj) { return obj.ToString(); }
};

std::ostream& operator<<(std::ostream& os, const IndirectString& str);

class StringBuffer
{
public:
	void Reset(uint32_t size);

	void Clear();

	void AppendFormat(const char* format, ...);

	void AppendIndirectString(const IndirectString& text);

	void AppendString(const std::string& text);

	void AppendMemory(void* memory, uint32_t size);

	void AppendStringBuffer(const StringBuffer& other_buffer);

	char* Allocate(uint32_t size);

	size_t Size() const;

	const char* CStr() const;

protected:
	std::vector<char> data_;
};

class DataBuffer
{
public:
	// Constructor
	DataBuffer(uint32_t max_entries, uint32_t buffer_size);

	// Destructor
	~DataBuffer() = default;

	void Reset();

	uint32_t AddData(double in_data);

	void GetData(uint32_t entry_index, float& value) const;

	void GetData(float& value) const;

	uint32_t GetLastEntryIndex() const;

	void Print()
	{
		for (uint32_t i = 0; i < current_size_; ++i)
		{
			float value;
			GetData(i, value);
			std::cout << value << std::endl;
		}
	}

protected:
	struct Entry
	{
		uint32_t offset : 30;
		uint32_t type : 2;

		Entry() : offset(0), type(0) {}
	};

	std::vector<Entry> entries_;
	std::vector<char> data_;
	uint32_t max_entries_;
	uint32_t current_entry_trail_index_; //Index the element after the last element
	uint32_t buffer_size_;
	uint32_t current_size_;
};

// No need for init_data_buffer and terminate_data_buffer functions
// as the constructor and destructor handle initialization and cleanup.

struct CodeFragment;

enum class TokenType
{
	kToken_Unknown,
	kToken_OpenParen,
	kToken_CloseParen,
	kToken_Colon,
	kToken_Semicolon,
	kToken_Asterisk,
	kToken_OpenBracket,
	kToken_CloseBracket,
	kToken_OpenBrace,
	kToken_CloseBrace,
	kToken_OpenAngleBracket,
	kToken_CloseAngleBracket,
	kToken_Equals,
	kToken_Hash,
	kToken_Comma,
	kToken_String,
	kToken_Identifier,
	kToken_Number,
	kToken_EndOfStream,
};

struct Token
{
	TokenType type_;
	IndirectString text_;
	typedef TokenType Type;
	uint32_t line_;

	void Init(char const* position, uint32_t in_line)
	{
		type_ = TokenType::kToken_Unknown;
		text_.text_ = position;
		text_.length_ = 1;
		line_ = in_line;
	}
};

struct Property
{
	std::string name_;
	std::string ui_name_;
	graphics::PropertyType type_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, Property& property);

	friend BinarySerializer& operator<<(BinarySerializer& serializer, std::shared_ptr<Property>& property_ptr);
};

struct IntProperty : Property
{
	int default_value_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, IntProperty& int_property);
};

struct FloatProperty : Property
{
	float default_value_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, FloatProperty& float_property);
};

struct RangeProperty : Property
{
	float min_value_;
	float max_value_;
	float default_value_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, RangeProperty& range_property);
};

struct TextureProperty : Property
{
	std::string default_value_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, TextureProperty& texture_property);
};

struct ResourceBinding
{
	std::string name_;
	graphics::ResourceType type_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, ResourceBinding& resource_binding);
};

struct ResourceList
{
	std::string name_;
	std::vector<ResourceBinding> resources_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, ResourceList& resource_list);
};

struct RenderState
{
	std::string name_;
	graphics::RasterizationState rasterization_state_;
	graphics::DepthStencilState depth_stencil_state_;
	graphics::BlendState blend_state_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, RenderState& render_state);
};

struct Shader
{
	graphics::ShaderType type_;
	int code_chunk_ref_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, Shader& shader);
};

struct Pass
{
	std::string name_;
	std::vector<Shader> shaders_;
	std::vector<int> resource_list_refs_;
	int render_state_ref_;
	graphics::PassType type_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, Pass& pass);
};

struct Resource
{
	graphics::ResourceType type_;
	std::string name_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, Resource& resource);
};

struct CodeChunk
{
	std::string name_;
	std::vector<std::string> includes_;
	std::vector<Resource> resources_; // represent the resource layout
	std::string code_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, CodeChunk& code_chunk);
};

class ShaderEffect
{
public:
	std::string name_;
	std::vector<Pass> passes_;
	std::vector<CodeChunk> code_chunks_;
	std::vector<ResourceList> resource_lists_; //represent the real resources
	std::vector<RenderState> render_states_;
	std::vector<std::shared_ptr<Property>> properties_;

	friend BinarySerializer& operator<<(BinarySerializer& serializer, ShaderEffect& shader_effect);

	friend std::ostream& operator<<(std::ostream& os, const ShaderEffect& shader_effect);

	graphics::ResourceListLayoutCreation CreateResourceListLayoutCreation(){return graphics::ResourceListLayoutCreation();}
	uint32_t GetLocalConstantsSize() const { return 0; }
};

#define INVALID_PROPERTY_DATA_INDEX 0xFFFFFFFF

class Lexer
{
public:
	Lexer(const std::string& source, DataBuffer& in_data_buffer);

	Lexer(const Lexer& other);

	Lexer& operator=(const Lexer& other);

	void GetTokenTextFromString(IndirectString& token);

	bool IsIdOrKeyword(char c);

	void NextToken(Token& token);

	bool EqualToken(Token& token, TokenType expected_type);

	// The same to equalToken but with error handling.
	bool ExpectToken(Token& token, TokenType expected_type);

	// Only check the token type.
	bool CheckToken(const Token& token, TokenType expected_type);

private:
	bool IsEndOfLine(char c);

	bool IsWhitespace(char c);

	bool IsAlpha(char c);

	bool IsNumber(char c);

private:
	void SkipWhitespaceAndComments();

	bool IsSingleLineComments();

	void SkipComments();

	bool IsCStyleComments();

	void SkipCStyleComments();

private:
	void ParseNumber();

	int32_t CheckSign(char c);

	void HeadingZero();

	int32_t HandleDecimalPart();

	void HandleFractionalPart(int32_t& fractional_part, int32_t& fractional_divisor);

	void HandleExponent();

protected:
	char const* position_;
	uint32_t line_;
	uint32_t column_;
	bool has_error_;
	uint32_t error_line_;
	DataBuffer& data_buffer_;
};

class Parser
{
public:
	explicit Parser(Lexer& in_lexer, DataBuffer& data_buffer): lexer(in_lexer), data_buffer(data_buffer) {}

protected:
	// AST ast_;
	ShaderEffect shader_effect_;
	Lexer& lexer;
	DataBuffer& data_buffer;

public:
	void Parse();

	ShaderEffect& GetShaderEffect() { return shader_effect_; }

	inline void DeclarationEffect();

	bool TryParseIfDefined(const Token& token, CodeChunk& code_chunk);

	bool TryParsePragma(const Token& token, CodeChunk& code_chunk);

	bool TryParseEndIf(const Token& token, CodeChunk& code_chunk);

	void DirectiveIdentifier(const Token& token, CodeChunk& code_chunk);

	void UniformIdentifier(const Token& token, CodeChunk& code_chunk);

	void ParseGlslContent(Token& token, CodeChunk& code_chunk);

	inline void DeclarationGlsl(); //TODO:remove inline

	inline void DeclarationShader(Shader& out_shader);

	inline void PassIdentifier(const Token& token, Pass& pass);

	inline void DeclarationPass();

	inline void DeclarationProperties();

	bool NumberAndIdentifier(Token& token);

	void ParsePropertyDefaultValue(std::shared_ptr<Property> property, Token token);

	void DeclarationProperty(const IndirectString& name);

	graphics::PropertyType PropertyTypeIdentifier(const Token& token);

	Lexer CacheLexer(const Lexer& lexer);

	inline void Identifier(const Token& token);

	int FindCodeChunk(const std::string& name);
};

class ShaderGenerator
{
public:
	ShaderGenerator(const ShaderEffect& shader_effect);

	void GenerateShaders(const std::string& path);

	void OutputShader(const std::string& path, const Shader& shader, const std::vector<CodeChunk>& code_chunks);

protected:
	const ShaderEffect& shader_effect_;
};

void CompileHFX(const std::string& file_path);
}
