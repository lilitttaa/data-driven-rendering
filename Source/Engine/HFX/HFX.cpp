#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>
#include "HFX.h"
#include "PathManager.h"
#include "File/FileReader.h"

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

bool ExpectKeyword(const IndirecString& text, const std::string& expected_keyword)
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

bool IndirecString::Equals(const IndirecString& a, const IndirecString& b)
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

std::ostream& operator<<(std::ostream& os, const IndirecString& str)
{
	for (size_t i = 0; i < str.length; ++i) { os << str.text[i]; }
	return os;
}

void AST::Print()
{
	std::cout << "Shader: " << name << std::endl;
	for (const auto& codeFragment : codeFragments)
	{
		std::cout << "CodeFragment: " << codeFragment.name << std::endl;
		std::cout << "Code: " << codeFragment.code << std::endl;
		std::cout << "Ifdef Depth: " << codeFragment.ifdefDepth << std::endl;
		std::cout << "Current Stage: " << static_cast<uint32_t>(codeFragment.currentStage) << std::endl;
		for (const auto& resource : codeFragment.resources) { std::cout << "Resource: " << resource.name << std::endl; }
		for (const auto& include : codeFragment.includes) { std::cout << "Include: " << include << std::endl; }
		for (const auto& include_flag : codeFragment.includesFlags) { std::cout << "Include Flag: " << include_flag << std::endl; }
		for (size_t i = 0; i < static_cast<uint32_t>(ShaderStage::Count); i++)
		{
			std::cout << "Stage Ifdef Depth: " << codeFragment.stageIfdefDepth[i] << std::endl;
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

std::map<char, TokenType> tokenMap = {
	{'\0', TokenType::Token_EndOfStream},
	{'(', TokenType::Token_OpenParen},
	{')', TokenType::Token_CloseParen},
	{':', TokenType::Token_Colon},
	{';', TokenType::Token_Semicolon},
	{'*', TokenType::Token_Asterisk},
	{'[', TokenType::Token_OpenBracket},
	{']', TokenType::Token_CloseBracket},
	{'{', TokenType::Token_OpenBrace},
	{'}', TokenType::Token_CloseBrace},
	{'=', TokenType::Token_Equals},
	{'#', TokenType::Token_Hash},
	{',', TokenType::Token_Comma},
};

void Lexer::GetTokenTextFromString(IndirecString& tokenText)
{
	tokenText.text = position;
	while (position[0] &&
	       position[0] != '"')
	{
		if ((position[0] == '\\') &&
		    position[1]) { ++position; }
		++position;
	}
	tokenText.length = static_cast<uint32_t>(position - tokenText.text);
	if (position[0] == '"') { ++position; }
}

bool Lexer::IsIdOrKeyword(char c) { return IsAlpha(c); }

void Lexer::NextToken(Token& token)
{
	SkipWhitespaceAndComments();
	token.Init(position, line);
	char c = position[0];
	++position;
	if (tokenMap.find(c) != tokenMap.end())
	{
		token.type = tokenMap[c];
		if (token.type == TokenType::Token_String) { GetTokenTextFromString(token.text); }
	}
	else
	{
		if (IsIdOrKeyword(c))
		{
			token.type = TokenType::Token_Identifier;
			while (IsAlpha(position[0]) || IsNumber(position[0]) || (position[0] == '_')) { ++position; }
			token.text.length = static_cast<uint32_t>(position - token.text.text);
		}
		else if (IsNumber(c) || c == '-')
		{
			token.type = TokenType::Token_Number;
			// Backtrack to start properly parsing the number
			--position;
			ParseNumber();
			token.text.length = static_cast<uint32_t>(position - token.text.text);
		}
		else { token.type = TokenType::Token_Unknown; }
	}
}

bool Lexer::EqualToken(Token& token, TokenType expected_type)
{
	NextToken(token);
	return token.type == expected_type;
}

bool Lexer::ExpectToken(Token& token, TokenType expected_type)
{
	if (hasError)
		return true;

	NextToken(token);
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

bool Lexer::IsSingleLineComments() { return (position[0] == '/') && (position[1] == '/'); }

void Lexer::SkipComments()
{
	position += 2;
	while (position[0] && !IsEndOfLine(position[0])) { ++position; }
}

bool Lexer::IsCStyleComments() { return (position[0] == '/') && (position[1] == '*'); }

void Lexer::SkipCStyleComments()
{
	position += 2;

	while (!((position[0] == '*') && (position[1] == '/')))
	{
		if (IsEndOfLine(position[0]))
			++line;

		++position;
	}

	if (position[0] == '*') { position += 2; }
}

void Lexer::SkipWhitespaceAndComments()
{
	while (true)
	{
		if (IsWhitespace(position[0]))
		{
			if (IsEndOfLine(position[0]))
				++line;

			++position;
		}
		else if (IsSingleLineComments()) { SkipComments(); }
		else if (IsCStyleComments()) { SkipCStyleComments(); }
		else { break; }
	}
}

int32_t Lexer::CheckSign(char c)
{
	int32_t sign = 1;
	if (c == '-')
	{
		sign = -1;
		++position;
	}
	return sign;
}

void Lexer::HeadingZero()
{
	if (*position == '0')
	{
		++position;

		while (*position == '0')
			++position;
	}
}

int32_t Lexer::HandleDecimalPart()
{
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
	return decimal_part;
}

void Lexer::HandleFractionalPart(int32_t& fractional_part, int32_t& fractional_divisor)
{
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
}

void Lexer::HandleExponent() { if (*position == 'e' || *position == 'E') { ++position; } }
// Parse the following literals:
// 58, -58, 0.003, 4e2, 123.456e-67, 0.1E4f
void Lexer::ParseNumber()
{
	int32_t sign = CheckSign(position[0]);
	// 00.003
	HeadingZero();
	int32_t decimal_part = HandleDecimalPart();
	int32_t fractional_part = 0;
	int32_t fractional_divisor = 1;
	HandleFractionalPart(fractional_part, fractional_divisor);
	HandleExponent();
	// double parsed_number = (double)sign * (decimal_part + ((double)fractional_part / fractional_divisor));
	// add_data(data_buffer, parsed_number); 
}

void Parser::GenerateAST()
{
	bool parsing = true;
	while (parsing)
	{
		Token token;
		lexer.NextToken(token);
		switch (token.type)
		{
			case TokenType::Token_Identifier:
			{
				Identifier(token);
				break;
			}

			case TokenType::Token_EndOfStream:
			{
				parsing = false;
				break;
			}
			default: break;
		}
	}
}

void Parser::DeclarationShader()
{
	Token token;
	if (!lexer.ExpectToken(token, TokenType::Token_Identifier)) { return; }

	ast.name = token.text;

	if (!lexer.ExpectToken(token, TokenType::Token_OpenBrace)) { return; }

	while (!lexer.EqualToken(token, TokenType::Token_CloseBrace)) { Identifier(token); }
}

void Parser::DirectiveIdentifier(const Token& token, CodeFragment& code_fragment)
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
				if (ExpectKeyword(token.text, "if"))
				{
					lexer.NextToken(new_token);

					if (ExpectKeyword(new_token.text, "defined"))
					{
						lexer.NextToken(new_token);

						// Use 0 as not set value for the ifdef depth.
						++code_fragment.ifdefDepth;

						if (ExpectKeyword(new_token.text, "VERTEX"))
						{
							code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Vertex)] = code_fragment.ifdefDepth;
							code_fragment.currentStage = ShaderStage::Vertex;
						}
						else if (ExpectKeyword(new_token.text, "FRAGMENT"))
						{
							code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Fragment)] = code_fragment.ifdefDepth;
							code_fragment.currentStage = ShaderStage::Fragment;
						}
						else if (ExpectKeyword(new_token.text, "COMPUTE"))
						{
							code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Compute)] = code_fragment.ifdefDepth;
							code_fragment.currentStage = ShaderStage::Compute;
						}
					}

					return;
				}
				break;
			}

			case 'p':
			{
				if (ExpectKeyword(token.text, "pragma"))
				{
					lexer.NextToken(new_token);

					if (ExpectKeyword(new_token.text, "include"))
					{
						lexer.NextToken(new_token);

						code_fragment.includes.emplace_back(new_token.text);
						code_fragment.includesFlags.emplace_back((uint32_t)code_fragment.currentStage);
					}
					else if (ExpectKeyword(new_token.text, "include_hfx"))
					{
						lexer.NextToken(new_token);

						code_fragment.includes.emplace_back(new_token.text);
						uint32_t flag = (uint32_t)code_fragment.currentStage | 0x10; // 0x10 = local hfx.
						code_fragment.includesFlags.emplace_back(flag);
					}

					return;
				}
				break;
			}

			case 'e':
			{
				if (ExpectKeyword(token.text, "endif"))
				{
					if (code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Vertex)] == code_fragment.ifdefDepth)
					{
						code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Vertex)] = 0xffffffff;
						code_fragment.currentStage = ShaderStage::Count;
					}
					else if (code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Fragment)] == code_fragment.ifdefDepth)
					{
						code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Fragment)] = 0xffffffff;
						code_fragment.currentStage = ShaderStage::Count;
					}
					else if (code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Compute)] == code_fragment.ifdefDepth)
					{
						code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Compute)] = 0xffffffff;
						code_fragment.currentStage = ShaderStage::Count;
					}

					--code_fragment.ifdefDepth;

					return;
				}
				break;
			}
		}
	}
}

void Parser::UniformIdentifier(const Token& token, CodeFragment& code_fragment)
{
	for (uint32_t i = 0; i < token.text.length; ++i)
	{
		char c = *(token.text.text + i);

		switch (c)
		{
			case 'i':
			{
				if (ExpectKeyword(token.text, "image2D"))
				{
					// Advance to next token to get the name
					Token name_token;
					lexer.NextToken(name_token);

					CodeFragment::Resource resource = {ResourceType::TextureRW, name_token.text};
					code_fragment.resources.emplace_back(resource);
				}
				break;
			}

			case 's':
			{
				if (ExpectKeyword(token.text, "sampler2D"))
				{
					// Advance to next token to get the name
					Token name_token;
					lexer.NextToken(name_token);

					CodeFragment::Resource resource = {ResourceType::Texture, name_token.text};
					code_fragment.resources.emplace_back(resource);
				}
				break;
			}
		}
	}
}

void Parser::DeclarationGlsl()
{
	// Parse name
	Token token;
	if (!lexer.ExpectToken(token, TokenType::Token_Identifier)) { return; }
	CodeFragment code_fragment = {};
	// Cache name string
	code_fragment.name = token.text;

	for (size_t i = 0; i < static_cast<uint32_t>(ShaderStage::Count); i++) { code_fragment.stageIfdefDepth[i] = 0xffffffff; }

	if (!lexer.ExpectToken(token, TokenType::Token_OpenBrace)) { return; }

	// Advance token and cache the starting point of the code.
	lexer.NextToken(token);
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
			lexer.NextToken(token);

			DirectiveIdentifier(token, code_fragment);
		}
		else if (token.type == TokenType::Token_Identifier)
		{
			// Parse uniforms to add resource dependencies if not explicit in the HFX file.
			if (ExpectKeyword(token.text, "uniform"))
			{
				lexer.NextToken(token);

				UniformIdentifier(token, code_fragment);
			}
		}

		// Only advance token when we are inside the glsl braces, otherwise will skip the following glsl part.
		if (open_braces)
			lexer.NextToken(token);
	}

	// Calculate code string length using the token before the last close brace.
	code_fragment.code.length = token.text.text - code_fragment.code.text;

	ast.codeFragments.emplace_back(code_fragment);
}

void Parser::DirectiveIdentifier(Parser* parser, const Token& token, CodeFragment& code_fragment)
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
				if (ExpectKeyword(token.text, "if"))
				{
					parser->lexer.NextToken(new_token);

					if (ExpectKeyword(new_token.text, "defined"))
					{
						parser->lexer.NextToken(new_token);

						// Use 0 as not set value for the ifdef depth.
						++code_fragment.ifdefDepth;

						if (ExpectKeyword(new_token.text, "VERTEX"))
						{
							code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Vertex)] = code_fragment.ifdefDepth;
							code_fragment.currentStage = ShaderStage::Vertex;
						}
						else if (ExpectKeyword(new_token.text, "FRAGMENT"))
						{
							code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Fragment)] = code_fragment.ifdefDepth;
							code_fragment.currentStage = ShaderStage::Fragment;
						}
						else if (ExpectKeyword(new_token.text, "COMPUTE"))
						{
							code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Compute)] = code_fragment.ifdefDepth;
							code_fragment.currentStage = ShaderStage::Compute;
						}
					}

					return;
				}
				break;
			}
			case 'p':
			{
				if (ExpectKeyword(token.text, "pragma"))
				{
					parser->lexer.NextToken(new_token);

					if (ExpectKeyword(new_token.text, "include"))
					{
						parser->lexer.NextToken(new_token);

						code_fragment.includes.emplace_back(new_token.text);
						code_fragment.includesFlags.emplace_back(static_cast<uint32_t>(code_fragment.currentStage));
					}

					return;
				}
				break;
			}
			case 'e':
			{
				if (ExpectKeyword(token.text, "endif"))
				{
					if (code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Vertex)] == code_fragment.ifdefDepth)
					{
						code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Vertex)] = 0xffffffff;
						code_fragment.currentStage = ShaderStage::Count;
					}
					else if (code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Fragment)] == code_fragment.ifdefDepth)
					{
						code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Fragment)] = 0xffffffff;
						code_fragment.currentStage = ShaderStage::Count;
					}
					else if (code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Compute)] == code_fragment.ifdefDepth)
					{
						code_fragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Compute)] = 0xffffffff;
						code_fragment.currentStage = ShaderStage::Count;
					}

					--code_fragment.ifdefDepth;

					return;
				}
				break;
			}
		}
	}
}

void Parser::DeclarationShaderStage(Pass::Stage& out_stage)
{
	Token token;
	if (!lexer.ExpectToken(token, TokenType::Token_Equals)) { return; }

	if (!lexer.ExpectToken(token, TokenType::Token_Identifier)) { return; }

	out_stage.code = FindCodeFragment(token.text);
}

void Parser::PassIdentifier(const Token& token, Pass& pass)
{
	// Scan the name to know which stage we are parsing    
	for (uint32_t i = 0; i < token.text.length; ++i)
	{
		char c = *(token.text.text + i);

		switch (c)
		{
			case 'c':
			{
				if (ExpectKeyword(token.text, "compute"))
				{
					Pass::Stage stage = {nullptr, ShaderStage::Compute};
					DeclarationShaderStage(stage);
					pass.shader_stages.emplace_back(stage);
					return;
				}
				break;
			}

			case 'v':
			{
				if (ExpectKeyword(token.text, "vertex"))
				{
					Pass::Stage stage = {nullptr, ShaderStage::Vertex};
					DeclarationShaderStage(stage);
					pass.shader_stages.emplace_back(stage);
					return;
				}
				break;
			}

			case 'f':
			{
				if (ExpectKeyword(token.text, "fragment"))
				{
					Pass::Stage stage = {nullptr, ShaderStage::Fragment};
					DeclarationShaderStage(stage);
					pass.shader_stages.emplace_back(stage);
					return;
				}
				break;
			}
		}
	}
}

void Parser::DeclarationPass()
{
	Token token;
	if (!lexer.ExpectToken(token, TokenType::Token_Identifier)) { return; }

	Pass pass = {};
	// Cache name string
	pass.name = token.text;

	if (!lexer.ExpectToken(token, TokenType::Token_OpenBrace)) { return; }
	while (!lexer.EqualToken(token, TokenType::Token_CloseBrace)) { PassIdentifier(token, pass); }
	ast.passes.emplace_back(pass);
}

void Parser::Identifier(const Token& token)
{
	for (uint32_t i = 0; i < token.text.length; ++i)
	{
		switch (token.text.text[i])
		{
			case 's':
			{
				if (ExpectKeyword(token.text, "shader"))
				{
					DeclarationShader();
					return;
				}

				break;
			}

			case 'g':
			{
				if (ExpectKeyword(token.text, "glsl"))
				{
					DeclarationGlsl();
					return;
				}
				break;
			}

			case 'p':
			{
				if (ExpectKeyword(token.text, "pass"))
				{
					DeclarationPass();
					return;
				}
				break;
			}
		}
	}
}

const CodeFragment* Parser::FindCodeFragment(const IndirecString& name)
{
	for (uint32_t i = 0; i < ast.codeFragments.size(); ++i)
	{
		const CodeFragment* type = &ast.codeFragments[i];
		if (IndirecString::Equals(name, type->name)) { return type; }
	}
	return nullptr;
}

void compileHFX(const std::string& filePath)
{
	Lexer lexer(FileReader(filePath).Read());
	Parser parser(lexer);
	parser.GenerateAST();
	AST& ast = parser.GetAST();
	ast.Print();
	ShaderGenerator shaderGenerator(ast);
	shaderGenerator.GenerateShaders(ST::PathManager::GetHFXDir());
}

ShaderGenerator::ShaderGenerator(const AST& ast): ast(ast), stringBuffers(3) {}

void ShaderGenerator::GenerateShaders(const std::string& path)
{
	stringBuffers[0].clear();
	stringBuffers[1].clear();
	stringBuffers[2].clear();

	// For each pass and for each pass generate permutation file.
	const uint32_t pass_count = (uint32_t)ast.passes.size();
	for (uint32_t i = 0; i < pass_count; i++)
	{
		// Create one file for each code fragment
		const Pass& pass = ast.passes[i];

		for (size_t s = 0; s < pass.shader_stages.size(); ++s) { OutputShaderStage(path, pass.shader_stages[s]); }
	}
}

void ShaderGenerator::OutputShaderStage(const std::string& path, const Pass::Stage& stage)
{
	const CodeFragment* code_fragment = stage.code;
	if (code_fragment == nullptr)
		return;

	std::string fileName = path + code_fragment->name.ToString() + "_" + ShaderStage2Postfix(stage.stage) + ".glsl";
	std::ofstream file(fileName);
	std::string code = code_fragment->code.ToString();
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
}
