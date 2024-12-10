#include <direct.h>
#include <iostream>
#include "Application.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "gtc/type_ptr.hpp"

using namespace ST;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::string GetProjectDir()
{
	char buffer[256];
	char* dicPath = _getcwd(buffer, sizeof(buffer));
	char* ret = dicPath;
	for (; *dicPath != '\0'; ++dicPath)
	{
		if (*dicPath == '\\')
			*dicPath = '/';
	}
	return std::string(ret) + "/../";
}

std::string GetShaderDir() { return GetProjectDir() + "Resource/OpenGLShader/"; }

std::string GetHFXDir() { return GetProjectDir() + "Resource/HFX/"; }

bool LoadFileToStr(std::string filePath, std::string& outStr)
{
	std::ifstream fileStream(filePath, std::ios::ate);
	if (!fileStream.is_open())
	{
		std::cout << "File Open Failed !" << filePath << '\n';
		return false;
	}
	long long endPos = fileStream.tellg();
	outStr.resize(endPos);
	fileStream.seekg(0, std::ios::beg);
	fileStream.read(&outStr[0], endPos);
	fileStream.close();
	return true;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

struct Ball
{
	float radius;
	float posX, posY;
	float velocityX, velocityY;
};

struct StringRef
{
	size_t length;
	char* text;

	static bool equals(const StringRef& a, const StringRef& b)
	{
		if (a.length != b.length)
			return false;
		for (size_t i = 0; i < a.length; ++i)
		{
			if (a.text[i] != b.text[i])
				return false;
		}
		return true;
	}

	static void copy(const StringRef& a, char* buffer, size_t buffer_size)
	{
		const size_t copy_size = a.length < buffer_size
		                         ? a.length
		                         : buffer_size;
		for (size_t i = 0; i < copy_size; ++i) { buffer[i] = a.text[i]; }
	}

	friend std::ostream& operator<<(std::ostream& os, const StringRef& str);
}; // struct StringRef

std::ostream& operator<<(std::ostream& os, const StringRef& str)
{
	for (size_t i = 0; i < str.length; ++i) { os << str.text[i]; }
	return os;
}

// typedef std::string StringRef; //TODO
// template <typename T>
// typedef std::vector<T> VectorRef; //TODO

namespace HFX
{
class Lexer;

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

std::ostream& operator<<(std::ostream& os, const ShaderStage& stage)
{
	switch (stage)
	{
		case ShaderStage::Vertex: { os << "Vertex"; }
		break;
		case ShaderStage::Fragment: { os << "Fragment"; }
		break;
		case ShaderStage::Geometry: { os << "Geometry"; }
		break;
		case ShaderStage::Compute: { os << "Compute"; }
		break;
		case ShaderStage::Hull: { os << "Hull"; }
		break;
		case ShaderStage::Domain: { os << "Domain"; }
		break;
		case ShaderStage::Count: { os << "Count"; }
		break;
	}
	return os;
}

bool expectKeyword(const StringRef& text, const std::string& expected_keyword)
{
	if (text.length != expected_keyword.length())
		return false;

	for (uint32_t i = 0; i < expected_keyword.length(); ++i)
	{
		if (text.text[i] != expected_keyword[i])
			return false;
	}
	return true;
}

class Lexer
{
public:
	Lexer(char* source): position(source), line(1), column(0), hasError(false), errorLine(1) {}

	bool IsEndOfLine(char c) { return (c == '\n' || c == '\r'); }
	bool IsWhitespace(char c) { return (c == ' ' || c == '\t' || IsEndOfLine(c)); }
	bool IsAlpha(char c) { return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')); }
	bool IsNumber(char c) { return (c >= '0' && c <= '9'); }

	void skipWhitespace()
	{
		// Scan text until whitespace is finished.
		for (;;)
		{
			// Check if it is a pure whitespace first.
			if (IsWhitespace(position[0]))
			{
				// Handle change of line
				if (IsEndOfLine(position[0]))
					++line;

				// Advance to next character
				++position;
			} // Check for single line comments ("//")
			else if ((position[0] == '/') && (position[1] == '/'))
			{
				position += 2;
				while (position[0] && !IsEndOfLine(position[0])) { ++position; }
			} // Check for c-style comments
			else if ((position[0] == '/') && (position[1] == '*'))
			{
				position += 2;

				// Advance until the string is closed. Remember to check if line is changed.
				while (!((position[0] == '*') && (position[1] == '/')))
				{
					// Handle change of line
					if (IsEndOfLine(position[0]))
						++line;

					// Advance to next character
					++position;
				}

				if (position[0] == '*') { position += 2; }
			}
			else { break; }
		}
	}

	void parse_number()
	{
		char c = position[0];

		// Parse the following literals:
		// 58, -58, 0.003, 4e2, 123.456e-67, 0.1E4f
		// 1. Sign detection
		int32_t sign = 1;
		if (c == '-')
		{
			sign = -1;
			++position;
		}

		// 2. Heading zeros (00.003)
		if (*position == '0')
		{
			++position;

			while (*position == '0')
				++position;
		}
		// 3. Decimal part (until the point)
		int32_t decimal_part = 0;
		if (*position > '0' && *position <= '9')
		{
			decimal_part = (*position - '0');
			++position;

			while (*position != '.' && IsNumber(*position))
			{
				decimal_part = (decimal_part * 10) + (*position - '0');

				++position;
			}
		}
		// 4. Fractional part
		int32_t fractional_part = 0;
		int32_t fractional_divisor = 1;

		if (*position == '.')
		{
			++position;

			while (IsNumber(*position))
			{
				fractional_part = (fractional_part * 10) + (*position - '0');
				fractional_divisor *= 10;

				++position;
			}
		}

		// 5. Exponent (if present)
		if (*position == 'e' || *position == 'E') { ++position; }

		double parsed_number = (double)sign * (decimal_part + ((double)fractional_part / fractional_divisor));
		// add_data(data_buffer, parsed_number); //TODO
	}

	void nextToken(Token& token)
	{
		// Skip all whitespace first so that the token is without them.
		skipWhitespace();

		// Initialize token
		token.type = TokenType::Token_Unknown;
		token.text.text = position;
		token.text.length = 1;
		token.line = line;

		char c = position[0];
		++position;

		switch (c)
		{
			case '\0': { token.type = TokenType::Token_EndOfStream; }
			break;
			case '(': { token.type = TokenType::Token_OpenParen; }
			break;
			case ')': { token.type = TokenType::Token_CloseParen; }
			break;
			case ':': { token.type = TokenType::Token_Colon; }
			break;
			case ';': { token.type = TokenType::Token_Semicolon; }
			break;
			case '*': { token.type = TokenType::Token_Asterisk; }
			break;
			case '[': { token.type = TokenType::Token_OpenBracket; }
			break;
			case ']': { token.type = TokenType::Token_CloseBracket; }
			break;
			case '{': { token.type = TokenType::Token_OpenBrace; }
			break;
			case '}': { token.type = TokenType::Token_CloseBrace; }
			break;
			case '=': { token.type = TokenType::Token_Equals; }
			break;
			case '#': { token.type = TokenType::Token_Hash; }
			break;
			case ',': { token.type = TokenType::Token_Comma; }
			break;

			case '"':
			{
				token.type = TokenType::Token_String;

				token.text.text = position;

				while (position[0] &&
				       position[0] != '"')
				{
					if ((position[0] == '\\') &&
					    position[1]) { ++position; }
					++position;
				}

				token.text.length = static_cast<uint32_t>(position - token.text.text);
				if (position[0] == '"') { ++position; }
			}
			break;

			default:
			{
				// Identifier/keywords
				if (IsAlpha(c))
				{
					token.type = TokenType::Token_Identifier;

					while (IsAlpha(position[0]) || IsNumber(position[0]) || (position[0] == '_')) { ++position; }

					token.text.length = static_cast<uint32_t>(position - token.text.text);
				} // Numbers: handle also negative ones!
				else if (IsNumber(c) || c == '-')
				{
					// Backtrack to start properly parsing the number
					--position;
					parse_number();
					// Update token and calculate correct length.
					token.type = TokenType::Token_Number;
					token.text.length = static_cast<uint32_t>(position - token.text.text);
				}
				else { token.type = TokenType::Token_Unknown; }
			}
			break;
		}
	}

	bool equalToken(Token& token, TokenType expected_type)
	{
		nextToken(token);
		return token.type == expected_type;
	}

	// The same to equalToken but with error handling.
	bool expectToken(Token& token, TokenType expected_type)
	{
		if (hasError)
			return true;

		nextToken(token);
		hasError = token.type != expected_type;
		if (hasError)
		{
			// Save line of error
			errorLine = line;
		}
		return !hasError;
	}

protected:
	char* position;
	uint32_t line;
	uint32_t column;
	bool hasError;
	uint32_t errorLine;
	StringRef dataBuffer; //TODO
};

enum class ResourceType
{
	Sampler, Texture, TextureRW, Constants, Buffer, BufferRW, Count
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

	void ShowShader()
	{
		std::cout << "Shader: " << name << std::endl;
		// std::cout << "Test" << std::endl;
		for (const auto& codeFragment : codeFragments)
		{
			std::cout << "CodeFragment: " << codeFragment.name << std::endl;
			std::cout << "Code: " << codeFragment.code << std::endl;
			std::cout << "Ifdef Depth: " << codeFragment.ifdef_depth << std::endl;
			std::cout << "Current Stage: " << static_cast<uint32_t>(codeFragment.current_stage) << std::endl;
			for (const auto& resource : codeFragment.resources) { std::cout << "Resource: " << resource.name << std::endl; }
			for (const auto& include : codeFragment.includes) { std::cout << "Include: " << include << std::endl; }
			for (const auto& include_flag : codeFragment.includes_flags) { std::cout << "Include Flag: " << include_flag << std::endl; }
			for (size_t i = 0; i < static_cast<uint32_t>(ShaderStage::Count); i++)
			{
				std::cout << "Stage Ifdef Depth: " << codeFragment.stage_ifdef_depth[i] << std::endl;
			}
		}
		for (const auto& pass : passes)
		{
			std::cout << "Pass: " << pass.name << std::endl;
			for (const auto& shader_stage : pass.shader_stages)
			{
				std::cout << shader_stage.stage << "---" << shader_stage.code->name << std::endl;
				std::cout << "shader_stage.code->code: " << shader_stage.code->code << std::endl;
			}
		}
	}
};

class Parser
{
public:
	Parser(Lexer& lexer): lexer(lexer) {}

	Shader shader;

protected:
	Lexer& lexer;

public:
	void generateAST()
	{
		// Read source text until the end.
		// The main body can be a list of declarations.
		bool parsing = true;

		while (parsing)
		{
			Token token;
			lexer.nextToken(token);

			switch (token.type)
			{
				case TokenType::Token_Identifier:
				{
					identifier(token);
					break;
				}

				case TokenType::Token_EndOfStream:
				{
					parsing = false;
					break;
				}
			}
		}
	}

	inline void declarationShader()
	{
		Token token;
		if (!lexer.expectToken(token, TokenType::Token_Identifier)) { return; }

		shader.name = token.text;

		if (!lexer.expectToken(token, TokenType::Token_OpenBrace)) { return; }

		while (!lexer.equalToken(token, TokenType::Token_CloseBrace)) { identifier(token); }
	}

	void directive_identifier(const Token& token, CodeFragment& code_fragment)
	{
		Token new_token;
		for (uint32_t i = 0; i < token.text.length; ++i)
		{
			char c = *(token.text.text + i);

			switch (c)
			{
				case 'i':
				{
					// Search for the pattern 'if defined'
					if (expectKeyword(token.text, "if"))
					{
						lexer.nextToken(new_token);

						if (expectKeyword(new_token.text, "defined"))
						{
							lexer.nextToken(new_token);

							// Use 0 as not set value for the ifdef depth.
							++code_fragment.ifdef_depth;

							if (expectKeyword(new_token.text, "VERTEX"))
							{
								code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Vertex)] = code_fragment.ifdef_depth;
								code_fragment.current_stage = ShaderStage::Vertex;
							}
							else if (expectKeyword(new_token.text, "FRAGMENT"))
							{
								code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Fragment)] = code_fragment.ifdef_depth;
								code_fragment.current_stage = ShaderStage::Fragment;
							}
							else if (expectKeyword(new_token.text, "COMPUTE"))
							{
								code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Compute)] = code_fragment.ifdef_depth;
								code_fragment.current_stage = ShaderStage::Compute;
							}
						}

						return;
					}
					break;
				}

				case 'p':
				{
					if (expectKeyword(token.text, "pragma"))
					{
						lexer.nextToken(new_token);

						if (expectKeyword(new_token.text, "include"))
						{
							lexer.nextToken(new_token);

							code_fragment.includes.emplace_back(new_token.text);
							code_fragment.includes_flags.emplace_back((uint32_t)code_fragment.current_stage);
						}
						else if (expectKeyword(new_token.text, "include_hfx"))
						{
							lexer.nextToken(new_token);

							code_fragment.includes.emplace_back(new_token.text);
							uint32_t flag = (uint32_t)code_fragment.current_stage | 0x10; // 0x10 = local hfx.
							code_fragment.includes_flags.emplace_back(flag);
						}

						return;
					}
					break;
				}

				case 'e':
				{
					if (expectKeyword(token.text, "endif"))
					{
						if (code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Vertex)] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Vertex)] = 0xffffffff;
							code_fragment.current_stage = ShaderStage::Count;
						}
						else if (code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Fragment)] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Fragment)] = 0xffffffff;
							code_fragment.current_stage = ShaderStage::Count;
						}
						else if (code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Compute)] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Compute)] = 0xffffffff;
							code_fragment.current_stage = ShaderStage::Count;
						}

						--code_fragment.ifdef_depth;

						return;
					}
					break;
				}
			}
		}
	}

	void uniform_identifier(const Token& token, CodeFragment& code_fragment)
	{
		for (uint32_t i = 0; i < token.text.length; ++i)
		{
			char c = *(token.text.text + i);

			switch (c)
			{
				case 'i':
				{
					if (expectKeyword(token.text, "image2D"))
					{
						// Advance to next token to get the name
						Token name_token;
						lexer.nextToken(name_token);

						CodeFragment::Resource resource = {ResourceType::TextureRW, name_token.text};
						code_fragment.resources.emplace_back(resource);
					}
					break;
				}

				case 's':
				{
					if (expectKeyword(token.text, "sampler2D"))
					{
						// Advance to next token to get the name
						Token name_token;
						lexer.nextToken(name_token);

						CodeFragment::Resource resource = {ResourceType::Texture, name_token.text};
						code_fragment.resources.emplace_back(resource);
					}
					break;
				}
			}
		}
	}

	inline void declarationGlsl()
	{
		// Parse name
		Token token;
		if (!lexer.expectToken(token, TokenType::Token_Identifier)) { return; }
		CodeFragment code_fragment = {};
		// Cache name string
		code_fragment.name = token.text;

		for (size_t i = 0; i < static_cast<uint32_t>(ShaderStage::Count); i++) { code_fragment.stage_ifdef_depth[i] = 0xffffffff; }

		if (!lexer.expectToken(token, TokenType::Token_OpenBrace)) { return; }

		// Advance token and cache the starting point of the code.
		lexer.nextToken(token);
		code_fragment.code = token.text;

		uint32_t open_braces = 1;

		// Scan until close brace token
		while (open_braces)
		{
			if (token.type == TokenType::Token_OpenBrace)
				++open_braces;
			else if (token.type == TokenType::Token_CloseBrace)
				--open_braces;

			// Parse hash for includes and defines
			if (token.type == TokenType::Token_Hash)
			{
				// Get next token and check which directive is
				lexer.nextToken(token);

				directive_identifier(token, code_fragment);
			}
			else if (token.type == TokenType::Token_Identifier)
			{
				// Parse uniforms to add resource dependencies if not explicit in the HFX file.
				if (expectKeyword(token.text, "uniform"))
				{
					lexer.nextToken(token);

					uniform_identifier(token, code_fragment);
				}
			}

			// Only advance token when we are inside the glsl braces, otherwise will skip the following glsl part.
			if (open_braces)
				lexer.nextToken(token);
		}

		// Calculate code string length using the token before the last close brace.
		code_fragment.code.length = token.text.text - code_fragment.code.text;

		shader.codeFragments.emplace_back(code_fragment);
	}

	inline void directiveIdentifier(Parser* parser, const Token& token, CodeFragment& code_fragment)
	{
		Token new_token;
		for (uint32_t i = 0; i < token.text.length; ++i)
		{
			char c = *(token.text.text + i);

			switch (c)
			{
				case 'i':
				{
					// Search for the pattern 'if defined'
					if (expectKeyword(token.text, "if"))
					{
						parser->lexer.nextToken(new_token);

						if (expectKeyword(new_token.text, "defined"))
						{
							parser->lexer.nextToken(new_token);

							// Use 0 as not set value for the ifdef depth.
							++code_fragment.ifdef_depth;

							if (expectKeyword(new_token.text, "VERTEX"))
							{
								code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Vertex)] = code_fragment.ifdef_depth;
								code_fragment.current_stage = ShaderStage::Vertex;
							}
							else if (expectKeyword(new_token.text, "FRAGMENT"))
							{
								code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Fragment)] = code_fragment.ifdef_depth;
								code_fragment.current_stage = ShaderStage::Fragment;
							}
							else if (expectKeyword(new_token.text, "COMPUTE"))
							{
								code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Compute)] = code_fragment.ifdef_depth;
								code_fragment.current_stage = ShaderStage::Compute;
							}
						}

						return;
					}
					break;
				}
				case 'p':
				{
					if (expectKeyword(token.text, "pragma"))
					{
						parser->lexer.nextToken(new_token);

						if (expectKeyword(new_token.text, "include"))
						{
							parser->lexer.nextToken(new_token);

							code_fragment.includes.emplace_back(new_token.text);
							code_fragment.includes_flags.emplace_back(static_cast<uint32_t>(code_fragment.current_stage));
						}

						return;
					}
					break;
				}
				case 'e':
				{
					if (expectKeyword(token.text, "endif"))
					{
						if (code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Vertex)] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Vertex)] = 0xffffffff;
							code_fragment.current_stage = ShaderStage::Count;
						}
						else if (code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Fragment)] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Fragment)] = 0xffffffff;
							code_fragment.current_stage = ShaderStage::Count;
						}
						else if (code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Compute)] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[static_cast<uint32_t>(ShaderStage::Compute)] = 0xffffffff;
							code_fragment.current_stage = ShaderStage::Count;
						}

						--code_fragment.ifdef_depth;

						return;
					}
					break;
				}
			}
		}
	}

	inline void declarationShaderStage(Pass::Stage& out_stage)
	{
		Token token;
		if (!lexer.expectToken(token, TokenType::Token_Equals)) { return; }

		if (!lexer.expectToken(token, TokenType::Token_Identifier)) { return; }

		out_stage.code = findCodeFragment(token.text);
	}

	inline void passIdentifier(const Token& token, Pass& pass)
	{
		// Scan the name to know which stage we are parsing    
		for (uint32_t i = 0; i < token.text.length; ++i)
		{
			char c = *(token.text.text + i);

			switch (c)
			{
				case 'c':
				{
					if (expectKeyword(token.text, "compute"))
					{
						Pass::Stage stage = {nullptr, ShaderStage::Compute};
						declarationShaderStage(stage);
						pass.shader_stages.emplace_back(stage);
						return;
					}
					break;
				}

				case 'v':
				{
					if (expectKeyword(token.text, "vertex"))
					{
						Pass::Stage stage = {nullptr, ShaderStage::Vertex};
						declarationShaderStage(stage);
						pass.shader_stages.emplace_back(stage);
						return;
					}
					break;
				}

				case 'f':
				{
					if (expectKeyword(token.text, "fragment"))
					{
						Pass::Stage stage = {nullptr, ShaderStage::Fragment};
						declarationShaderStage(stage);
						pass.shader_stages.emplace_back(stage);
						return;
					}
					break;
				}
			}
		}
	}

	inline void declarationPass()
	{
		Token token;
		if (!lexer.expectToken(token, TokenType::Token_Identifier)) { return; }

		Pass pass = {};
		// Cache name string
		pass.name = token.text;

		if (!lexer.expectToken(token, TokenType::Token_OpenBrace)) { return; }
		while (!lexer.equalToken(token, TokenType::Token_CloseBrace)) { passIdentifier(token, pass); }
		shader.passes.emplace_back(pass);
	}

	inline void identifier(const Token& token)
	{
		// Scan the name to know which 
		for (uint32_t i = 0; i < token.text.length; ++i)
		{
			switch (token.text.text[i])
			{
				case 's':
				{
					if (expectKeyword(token.text, "shader"))
					{
						declarationShader();
						return;
					}

					break;
				}

				case 'g':
				{
					if (expectKeyword(token.text, "glsl"))
					{
						declarationGlsl();
						return;
					}
					break;
				}

				case 'p':
				{
					if (expectKeyword(token.text, "pass"))
					{
						declarationPass();
						return;
					}
					break;
				}
			}
		}
	}

	const CodeFragment* findCodeFragment(const StringRef& name)
	{
		for (uint32_t i = 0; i < shader.codeFragments.size(); ++i)
		{
			const CodeFragment* type = &shader.codeFragments[i];
			if (StringRef::equals(name, type->name)) { return type; }
		}
		return nullptr;
	}
};

class CodeGenerator
{
public:
	CodeGenerator(Parser& parser): parser(parser), string_buffers(3) {}

	void generateShaderPermutations(const std::string& path)
	{
		string_buffers[0].clear();
		string_buffers[1].clear();
		string_buffers[2].clear();

		// For each pass and for each pass generate permutation file.
		const uint32_t pass_count = (uint32_t)parser.shader.passes.size();
		for ( uint32_t i = 0; i < pass_count; i++ ) {

			// Create one file for each code fragment
			const Pass& pass = parser.shader.passes[i];

			for ( size_t s = 0; s < pass.shader_stages.size(); ++s ) {
				output_shader_stage( path, pass.shader_stages[s] );
			}
		}
	}

	void output_shader_stage(const std::string& path, const Pass::Stage& stage)
    {
        const CodeFragment* code_fragment = stage.code;
        if (code_fragment == nullptr)
            return;

        // Generate the permutation file
        // const char* stage_name = get_shader_stage_name( stage.stage );
        // const char* stage_extension = get_shader_stage_extension( stage.stage );

        // StringRef::copy( code_fragment->code, string_buffers[0].data(), string_buffers[0].size() );
        // StringRef::copy( code_fragment->name, string_buffers[1].data(), string_buffers[1].size() );

        // const char* output_path = path + string_buffers[1].data() + "_" + stage_name + stage_extension;
        // write_file( output_path, string_buffers[0].data(), string_buffers[0].size() );
    }

protected:
	Parser& parser;
	std::vector<std::string> string_buffers;
};
}

void compileHFX()
{
	std::string simpleFullscreenSourceStr;
	LoadFileToStr(GetHFXDir() + "SimpleFullscreen.hfx", simpleFullscreenSourceStr);
	// copy the string into a text char*
	uint32_t allocated_size = simpleFullscreenSourceStr.size() + 1;
	char* text = new char[allocated_size]; //TODO: free this memory
	memcpy(text, simpleFullscreenSourceStr.c_str(), allocated_size);

	HFX::Lexer lexer(text);
	HFX::Parser parser(lexer);
	parser.generateAST();
	parser.shader.ShowShader();

	delete[] text;

	HFX::CodeGenerator code_generator(parser);
	code_generator.generateShaderPermutations("");

	// const char* vertexShaderSource = vertexShaderSourceStr.c_str();

	// text = ReadEntireFileIntoMemory("SimpleFullscreen.hfx", nullptr);
	// initLexer(&lexer, (char*)text);
	//
	// hfx::Parser effect_parser;
	// hfx::initParser(&effect_parser, &lexer);
	// hfx::generateAST(&effect_parser);
	//
	// hfx::CodeGenerator hfx_code_generator;
	// hfx::initCodeGenerator(&hfx_code_generator, &effect_parser, 4096);
	// hfx::generateShaderPermutations(&hfx_code_generator, "..\\data\\");
}

class Example1 : public ST::Application
{
public:
	Example1(): ST::Application() {}

	~Example1() override {}

	virtual void Init() override
	{
// glfw: initialize and configure
		// ------------------------------
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// glfw window creation
		// --------------------
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}

		std::string vertexShaderSourceStr;
		LoadFileToStr(GetShaderDir() + "Ball.vt.glsl", vertexShaderSourceStr);
		const char* vertexShaderSource = vertexShaderSourceStr.c_str();

		std::string fragmentShaderSourceStr;
		LoadFileToStr(GetShaderDir() + "Ball.fg.glsl", fragmentShaderSourceStr);
		const char* fragmentShaderSource = fragmentShaderSourceStr.c_str();

		// build and compile our shader program
		// ------------------------------------
		// vertex shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		// check for shader compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// fragment shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		// check for shader compile errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// link shaders
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		// check for linking errors
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float vertices[] = {
			1.0f, 1.0f, 0.0f, // top right
			1.0f, -1.0f, 0.0f, // bottom right
			-1.0f, -1.0f, 0.0f, // bottom left
			-1.0f, 1.0f, 0.0f // top left 
		};
		unsigned int indices[] = {
			// note that we start from 0!
			0, 1, 3, // first Triangle
			1, 2, 3 // second Triangle
		};
		// unsigned int VBO, VAO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);

		// uncomment this call to draw in wireframe polygons.
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	virtual void Tick(float deltaTime) override
	{
		processInput(window);
		glfwPollEvents();

		ball.velocityY += gravity * deltaTime;
		ball.posX += ball.velocityX * deltaTime;
		ball.posY += ball.velocityY * deltaTime;

		if (ball.posX - ball.radius < 0.0f)
		{
			ball.posX = ball.radius;
			ball.velocityX = -ball.velocityX;
		}
		if (ball.posX + ball.radius > SCR_WIDTH)
		{
			ball.posX = SCR_WIDTH - ball.radius;
			ball.velocityX = -ball.velocityX;
		}
		if (ball.posY + ball.radius > SCR_HEIGHT)
		{
			ball.posY = SCR_HEIGHT - ball.radius;
			ball.velocityY = -ball.velocityY;
		}
		if (ball.posY - ball.radius < 0.0f)
		{
			ball.posY = ball.radius;
			ball.velocityY = -ball.velocityY;
		}
	}

	virtual void Render(float deltaTime) override
	{
		// render
		// ------
		// print ball position
		std::cout << "Ball Position: " << ball.posX << ", " << ball.posY << std::endl;
		glUniform2f(glGetUniformLocation(shaderProgram, "ballPos"), ball.posX, ball.posY);
		glUniform1f(glGetUniformLocation(shaderProgram, "ballRadius"), ball.radius);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw our first triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0); // no need to unbind it every time 

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
	}

	virtual void Destroy() override
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteProgram(shaderProgram);
		window = nullptr;
		glfwTerminate();
	}

protected:
	GLFWwindow* window;
	unsigned int VBO, VAO, EBO;
	unsigned int shaderProgram;
	unsigned int vertexShader;
	unsigned int fragmentShader;

	float gravity = 10.0f;
	float timeStep = 1.0f / 60.0f;
	Ball ball = {100.0f, 300.0f, 300.0f, 1000.0f, 150.0f};
};

class Example2 : public ST::Application
{
public:
	Example2(): ST::Application() {}

	~Example2() override {}

	virtual void Init() override { compileHFX(); }

	virtual void Tick(float deltaTime) override {}

	virtual void Render(float deltaTime) override {}

	virtual void Destroy() override {}
};

ST::Application* CreateApplication() { return new Example2(); }
