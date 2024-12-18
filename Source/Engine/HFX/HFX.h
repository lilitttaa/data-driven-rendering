#pragma once

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
};

std::ostream& operator<<(std::ostream& os, const IndirectString& str);

class StringBuffer
{
public:
	void Reset(uint32_t size);

	void Clear();

	void AppendFormat(const char* format, ...);

	void AppendIndirectString(const IndirectString& text);

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

enum class ShaderStage
{
	kVertex = 0, kFragment, kGeometry, kCompute, kHull, kDomain, kCount
};

enum class ResourceType
{
	kSampler, kTexture, kTextureRW, kConstants, kBuffer, kBufferRW, kCount
};

struct Pass
{
	struct Stage
	{
		const CodeFragment* code = nullptr;
		ShaderStage stage = ShaderStage::kCount;
	};

	IndirectString name_;
	// StringRef stage_name;
	std::vector<Stage> shader_stages_;
	//
	// std::vector<const ResourceList*> resource_lists; // List used by the pass
	// const VertexLayout* vertex_layout;
	// const RenderState* render_state;
};

enum PropertyType
{
	kFloat, kInt, kRange, kColor, kVector, kTexture1D, kTexture2D, kTexture3D, kTextureVolume, kUnknown
};

#define INVALID_PROPERTY_DATA_INDEX 0xFFFFFFFF

struct Property
{
	IndirectString name_;
	IndirectString ui_name_;
	IndirectString default_value_;
	PropertyType type_;
	uint32_t offset_in_bytes_ = 0;
	uint32_t data_index_ = INVALID_PROPERTY_DATA_INDEX;
};

struct AST
{
	IndirectString name_;
	std::vector<CodeFragment> code_fragments_;
	std::vector<Pass> passes_;
	std::vector<Property*> properties_;

	void Print();
};

struct CodeFragment
{
	struct Resource
	{
		ResourceType type_;
		IndirectString name_;
	};

	std::vector<IndirectString> includes_;
	std::vector<uint32_t> includes_flags_; // TODO: what is this?
	std::vector<Resource> resources_; // Resources used in the code.

	IndirectString name_;
	IndirectString code_;
	ShaderStage current_stage_ = ShaderStage::kCount;
	uint32_t if_def_depth_ = 0;
	uint32_t stage_if_def_depth_[ShaderStage::kCount];
};

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
	explicit Parser(Lexer& in_lexer, DataBuffer& data_buffer): ast_(), lexer(in_lexer), data_buffer(data_buffer) {}

protected:
	AST ast_;
	Lexer& lexer;
	DataBuffer& data_buffer;

public:
	void GenerateAST();

	AST& GetAST() { return ast_; }

	inline void DeclarationShader();

	bool TryParseIfDefined(const Token& token, CodeFragment& code_fragment);

	bool TryParsePragma(const Token& token, CodeFragment& code_fragment);

	bool TryParseEndIf(const Token& token, CodeFragment& code_fragment);

	void DirectiveIdentifier(const Token& token, CodeFragment& code_fragment);

	void UniformIdentifier(const Token& token, CodeFragment& code_fragment);

	void ParseGlslContent(Token& token, CodeFragment code_fragment);

	inline void DeclarationGlsl(); //TODO:remove inline

	inline void DeclarationShaderStage(Pass::Stage& out_stage);

	inline void PassIdentifier(const Token& token, Pass& pass);

	inline void DeclarationPass();

	inline void DeclarationProperties();

	bool NumberAndIdentifier(Token& token);

	void ParsePropertyDefaultValue(Property* property, Token token);

	void DeclarationProperty(const IndirectString& name);

	PropertyType PropertyTypeIdentifier(const Token& token);

	Lexer CacheLexer(const Lexer& lexer);

	inline void Identifier(const Token& token);

	const CodeFragment* FindCodeFragment(const IndirectString& name);
};

class ShaderGenerator
{
public:
	ShaderGenerator(const AST& ast);

	void GenerateShaders(const std::string& path);

	void OutputShaderStage(const std::string& path, const Pass::Stage& stage);

protected:
	const AST& ast;
};

void CompileHFX(const std::string& file_path);

void GeneatePropertiesShaderCodeAndGetDefault(AST& ast, const DataBuffer& data_buffer, StringBuffer& out_defaults, StringBuffer& out_buffer); //TODO
}
