#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>
#include "HFX.h"

#include "File/FileReader.h"

// typedef std::string StringRef; //TODO
// template <typename T>
// typedef std::vector<T> VectorRef; //TODO

namespace HFX
{
std::string shaderStageTable[] = {"VERTEX", "FRAGMENT", "GEOMETRY", "COMPUTE", "HULL", "DOMAIN"};
std::string shaderStagePostfixTable[] = {"vt", "fg", "gm", "cp", "hl", "dm"};

std::string ShaderStage2Postfix(ShaderStage stage) { return shaderStagePostfixTable[static_cast<uint32_t>(stage)]; }

std::string ShaderStage2String(ShaderStage stage) { return shaderStageTable[static_cast<uint32_t>(stage)]; }

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

bool StringRef::equals(const StringRef& a, const StringRef& b)
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

std::ostream& operator<<(std::ostream& os, const StringRef& str)
{
	for (size_t i = 0; i < str.length; ++i) { os << str.text[i]; }
	return os;
}

void Shader::ShowShader()
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

Lexer::Lexer(const std::string& source): position(source.c_str()), line(1), column(0), hasError(false), errorLine(1) {}

void Lexer::nextToken(Token& token)
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
				parseNumber();
				// Update token and calculate correct length.
				token.type = TokenType::Token_Number;
				token.text.length = static_cast<uint32_t>(position - token.text.text);
			}
			else { token.type = TokenType::Token_Unknown; }
		}
		break;
	}
}

bool Lexer::equalToken(Token& token, TokenType expected_type)
{
	nextToken(token);
	return token.type == expected_type;
}

bool Lexer::expectToken(Token& token, TokenType expected_type)
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

bool Lexer::IsEndOfLine(char c) { return (c == '\n' || c == '\r'); }

bool Lexer::IsWhitespace(char c) { return (c == ' ' || c == '\t' || IsEndOfLine(c)); }

bool Lexer::IsAlpha(char c) { return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')); }

bool Lexer::IsNumber(char c) { return (c >= '0' && c <= '9'); }

void Lexer::skipWhitespace()
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

void Lexer::parseNumber()
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

void Parser::generateAST()
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

void Parser::declarationShader()
{
	Token token;
	if (!lexer.expectToken(token, TokenType::Token_Identifier)) { return; }

	shader.name = token.text;

	if (!lexer.expectToken(token, TokenType::Token_OpenBrace)) { return; }

	while (!lexer.equalToken(token, TokenType::Token_CloseBrace)) { identifier(token); }
}

void Parser::directive_identifier(const Token& token, CodeFragment& code_fragment)
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

void Parser::uniform_identifier(const Token& token, CodeFragment& code_fragment)
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

void Parser::declarationGlsl()
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

void Parser::directiveIdentifier(Parser* parser, const Token& token, CodeFragment& code_fragment)
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

void Parser::declarationShaderStage(Pass::Stage& out_stage)
{
	Token token;
	if (!lexer.expectToken(token, TokenType::Token_Equals)) { return; }

	if (!lexer.expectToken(token, TokenType::Token_Identifier)) { return; }

	out_stage.code = findCodeFragment(token.text);
}

void Parser::passIdentifier(const Token& token, Pass& pass)
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

void Parser::declarationPass()
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

void Parser::identifier(const Token& token)
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

const CodeFragment* Parser::findCodeFragment(const StringRef& name)
{
	for (uint32_t i = 0; i < shader.codeFragments.size(); ++i)
	{
		const CodeFragment* type = &shader.codeFragments[i];
		if (StringRef::equals(name, type->name)) { return type; }
	}
	return nullptr;
}

void compileHFX(const std::string& filePath) {
	// Load HFX file
	FileReader fileReader(filePath);
	std::string hfxSourceStr = fileReader.Read();

	Lexer lexer(hfxSourceStr);
	// HFX::Parser parser(lexer);
	// HFX::AST ast = parser.generateAST();
	// ast.Print();
	// HFX::ShaderGenerator shaderGenerator(ast);
	// shaderGenerator.Generate();
	// for (auto& shader : shaderGenerator.GetShaders())
	// {
	// 	std::string fileName = shader.GetName() + ".glsl";
	// 	FileWriter fileWriter(fileName);
	// 	fileWriter.Write(shader.GetSource());
	// }

	// Init Lexer
	// Init Parser
	// Generate AST
	// Print AST
	// Generate Shaders

	// std::string simpleFullscreenSourceStr;
	// LoadFileToStr(GetHFXDir() + "Ball.hfx", simpleFullscreenSourceStr);
	// // copy the string into a text char*
	// uint32_t allocated_size = simpleFullscreenSourceStr.size() + 1;
	// char* text = new char[allocated_size]; //TODO: free this memory
	// memcpy(text, simpleFullscreenSourceStr.c_str(), allocated_size);
	//
	// HFX::Lexer lexer(text);
	// HFX::Parser parser(lexer);
	// parser.generateAST();
	// parser.shader.ShowShader();
	//
	// HFX::CodeGenerator code_generator(parser);
	// code_generator.generateShaderPermutations(GetHFXDir());
	// delete[] text;
}

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
		for (uint32_t i = 0; i < pass_count; i++)
		{
			// Create one file for each code fragment
			const Pass& pass = parser.shader.passes[i];

			for (size_t s = 0; s < pass.shader_stages.size(); ++s) { output_shader_stage(path, pass.shader_stages[s]); }
		}
	}

	void output_shader_stage(const std::string& path, const Pass::Stage& stage)
	{
		const CodeFragment* code_fragment = stage.code;
		if (code_fragment == nullptr)
			return;

		std::string fileName = path + code_fragment->name.to_string() + "_" + ShaderStage2Postfix(stage.stage) + ".glsl";
		std::ofstream file(fileName);
		std::string code = code_fragment->code.to_string();
		file << "#version 330 core\n";
		file << "#define " << ShaderStage2String(stage.stage) << "\n";
		file << code;
		file.close();

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


