namespace HFX
{
class IndirecString
{
public:
	size_t length;
	char const* text;

	static bool Equals(const IndirecString& a, const IndirecString& b);

	static void Copy(const IndirecString& a, char* buffer, size_t bufferSize)
	{
		const size_t copy_size = a.length < bufferSize
		                         ? a.length
		                         : bufferSize;
		for (size_t i = 0; i < copy_size; ++i) { buffer[i] = a.text[i]; }
	}

	friend std::ostream& operator<<(std::ostream& os, const IndirecString& str);

	std::string ToString() const
	{
		std::string str;
		for (size_t i = 0; i < length; ++i) { str += text[i]; }
		return str;
	}
};

std::ostream& operator<<(std::ostream& os, const IndirecString& str);

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
	IndirecString text;
	typedef TokenType Type;
	uint32_t line;

	void Init(char const* position, uint32_t inLine)
	{
		type = TokenType::Token_Unknown;
		text.text = position;
		text.length = 1;
		line = inLine;
	}
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
	};

	IndirecString name;
	// StringRef stage_name;
	std::vector<Stage> shaderStages;
	//
	// std::vector<const ResourceList*> resource_lists; // List used by the pass
	// const VertexLayout* vertex_layout;
	// const RenderState* render_state;
};

struct AST
{
	IndirecString name;
	std::vector<CodeFragment> codeFragments;
	std::vector<Pass> passes;

	void Print();
};

struct CodeFragment
{
	struct Resource
	{
		ResourceType type;
		IndirecString name;
	};

	std::vector<IndirecString> includes;
	std::vector<uint32_t> includesFlags; // TODO: what is this?
	std::vector<Resource> resources; // Resources used in the code.

	IndirecString name;
	IndirecString code;
	ShaderStage currentStage = ShaderStage::Count;
	uint32_t ifdefDepth = 0;
	uint32_t stageIfdefDepth[ShaderStage::Count];
};

class Lexer
{
public:
	Lexer(const std::string& source);

	void GetTokenTextFromString(IndirecString& token);

	bool IsIdOrKeyword(char c);

	void NextToken(Token& token);

	bool EqualToken(Token& token, TokenType expected_type);

	// The same to equalToken but with error handling.
	bool ExpectToken(Token& token, TokenType expected_type);

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

	void HandleFractionalPart(int32_t& fractionalPart, int32_t& fractionalDivisor);

	void HandleExponent();

protected:
	char const* position;
	uint32_t line;
	uint32_t column;
	bool hasError;
	uint32_t errorLine;
};

class Parser
{
public:
	explicit Parser(Lexer& inLexer): ast(), lexer(inLexer) {}

protected:
	AST ast;
	Lexer& lexer;

public:
	void GenerateAST();

	AST& GetAST() { return ast; }

	inline void DeclarationShader();

	bool TryParseIfDefined(const Token& token, CodeFragment& codeFragment);

	bool TryParsePragma(const Token& token, CodeFragment& codeFragment);

	bool TryParseEndIf(const Token& token, CodeFragment& codeFragment);

	void DirectiveIdentifier(const Token& token, CodeFragment& codeFragment);
	
	void UniformIdentifier(const Token& token, CodeFragment& codeFragment);

	void ParseGlslContent(Token& token, CodeFragment codeFragment);

	inline void DeclarationGlsl();

	inline void DeclarationShaderStage(Pass::Stage& out_stage);

	inline void PassIdentifier(const Token& token, Pass& pass);

	inline void DeclarationPass();

	inline void Identifier(const Token& token);

	const CodeFragment* FindCodeFragment(const IndirecString& name);
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

void compileHFX(const std::string& filePath);
void CompileShaderEffectFile();
}
