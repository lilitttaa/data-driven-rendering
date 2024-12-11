#include <contrib/utf8cpp/source/utf8/core.h>

namespace HFX
{
class StringRef
{
public:
	size_t length;
	char const* text;

	static bool equals(const StringRef& a, const StringRef& b);

	static void copy(const StringRef& a, char* buffer, size_t buffer_size)
	{
		const size_t copy_size = a.length < buffer_size
		                         ? a.length
		                         : buffer_size;
		for (size_t i = 0; i < copy_size; ++i) { buffer[i] = a.text[i]; }
	}

	friend std::ostream& operator<<(std::ostream& os, const StringRef& str);

	std::string to_string() const
	{
		std::string str;
		for (size_t i = 0; i < length; ++i) { str += text[i]; }
		return str;
	}
}; // struct StringRef

std::ostream& operator<<(std::ostream& os, const StringRef& str);

struct CodeFragment;

enum class TokenType
{
	Token_Unknown,
	Token_OpenParen,
	Token_CloseParen,
	Token_Colon,
	Token_Semicolon,
	Token_Asterisk,
	Token_OpenBracket,
	Token_CloseBracket,
	Token_OpenBrace,
	Token_CloseBrace,
	Token_OpenAngleBracket,
	Token_CloseAngleBracket,
	Token_Equals,
	Token_Hash,
	Token_Comma,
	Token_String,
	Token_Identifier,
	Token_Number,
	Token_EndOfStream,
};

struct Token
{
	TokenType type;

	StringRef text;
	// const char* text;
	// uint32_t length;
	typedef TokenType Type;
	uint32_t line;
};

enum class ShaderStage
{
	Vertex = 0, Fragment, Geometry, Compute, Hull, Domain, Count
};

enum class ResourceType
{
	Sampler, Texture, TextureRW, Constants, Buffer, BufferRW, Count
};

struct Pass
{
	struct Stage
	{
		const CodeFragment* code = nullptr;
		ShaderStage stage = ShaderStage::Count;
	}; // struct ShaderStage
	//
	StringRef name;
	// StringRef stage_name;
	std::vector<Stage> shader_stages;
	//
	// std::vector<const ResourceList*> resource_lists; // List used by the pass
	// const VertexLayout* vertex_layout;
	// const RenderState* render_state;
}; // struct Pass

struct Shader
{
	StringRef name;
	std::vector<CodeFragment> codeFragments;
	std::vector<Pass> passes;

	void ShowShader();
};

struct CodeFragment
{
	struct Resource
	{
		ResourceType type;
		StringRef name;
	};

	std::vector<StringRef> includes;
	std::vector<uint32_t> includes_flags; // TODO: what is this?
	std::vector<Resource> resources; // Resources used in the code.

	StringRef name;
	StringRef code;
	ShaderStage current_stage = ShaderStage::Count;
	uint32_t ifdef_depth = 0;
	uint32_t stage_ifdef_depth[ShaderStage::Count];
};

class Lexer
{
public:
	Lexer(const std::string& source);

	void nextToken(Token& token);

	bool equalToken(Token& token, TokenType expected_type);

	// The same to equalToken but with error handling.
	bool expectToken(Token& token, TokenType expected_type);

private:
	bool IsEndOfLine(char c);

	bool IsWhitespace(char c);

	bool IsAlpha(char c);

	bool IsNumber(char c);

	void parseNumber();

private:
	void SkipWhitespaceAndComments();

	bool IsSingleLineComments();

	void SkipComments();

	bool IsCStyleComments();

	void SkipCStyleComments();

protected:
	char const* position;
	utf8::uint32_t line;
	uint32_t column;
	bool hasError;
	uint32_t errorLine;
};

struct AST
{};

class Parser
{
public:
	explicit Parser(Lexer& lexer): shader(), lexer(lexer) {}

	Shader shader;

protected:
	AST ast;
	Lexer& lexer;

public:
	void generateAST();

	inline void declarationShader();

	void directive_identifier(const Token& token, CodeFragment& code_fragment);

	void uniform_identifier(const Token& token, CodeFragment& code_fragment);

	inline void declarationGlsl();

	inline void directiveIdentifier(Parser* parser, const Token& token, CodeFragment& code_fragment);

	inline void declarationShaderStage(Pass::Stage& out_stage);

	inline void passIdentifier(const Token& token, Pass& pass);

	inline void declarationPass();

	inline void identifier(const Token& token);

	const CodeFragment* findCodeFragment(const StringRef& name);
};

void compileHFX(const std::string& filePath);
}
