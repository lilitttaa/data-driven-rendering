#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>
#include "HFX.h"

#include <cstdarg>

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

void StringBuffer::Reset(uint32_t size)
{
	data.reserve(size);
	data.clear();
}

void StringBuffer::Clear() { data.clear(); }

#define STRING_BUFFER_RESERVE_SIZE 1024

void StringBuffer::AppendFormat(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::vector<char> temp;
	temp.resize(STRING_BUFFER_RESERVE_SIZE);
	while (true)
	{
		va_list argsCopy;
		argsCopy = args;
		int writtenChars = vsnprintf(temp.data(), temp.size(), format, argsCopy);
		if (writtenChars >= 0 && writtenChars < static_cast<int>(temp.size()))
		{
			data.insert(data.end(), temp.begin(), temp.begin() + writtenChars);
			break;
		}
		else { temp.resize(temp.size() * 2); }
	}
	va_end(args);
}

void StringBuffer::AppendIndirectString(const IndirecString& text)
{
	if (text.length > 0) { data.insert(data.end(), text.text, text.text + text.length); }
}

void StringBuffer::AppendMemory(void* memory, uint32_t size)
{
	if (size > 0) { data.insert(data.end(), static_cast<char*>(memory), static_cast<char*>(memory) + size); }
}

void StringBuffer::AppendStringBuffer(const StringBuffer& other_buffer)
{
	data.insert(data.end(), other_buffer.data.begin(), other_buffer.data.end());
}

char* StringBuffer::Allocate(uint32_t size)
{
	if (data.capacity() - data.size() < size) { data.reserve(data.capacity() + size); }
	size_t offset = data.size();
	data.resize(data.size() + size);
	return data.data() + offset;
}

size_t StringBuffer::Size() const { return data.size(); }
const char* StringBuffer::CStr() const { return data.data(); }

DataBuffer::DataBuffer(uint32_t maxEntries, uint32_t bufferSize): entries(maxEntries), data(bufferSize, '\0'),
	maxEntries(maxEntries), currentEntryTrailIndex(0),
	bufferSize(bufferSize), currentSize(0) {}

void DataBuffer::Reset()
{
	currentSize = 0;
	currentEntryTrailIndex = 0;
}

uint32_t DataBuffer::AddData(double inData)
{
	if (currentEntryTrailIndex >= maxEntries)
		return -1;

	if (currentSize + sizeof(double) >= bufferSize)
		return -1;

	// Init entry
	Entry& entry = entries[currentEntryTrailIndex++];
	entry.offset = currentSize;

	// Copy data
	std::memcpy(&data[currentSize], &inData, sizeof(double));
	currentSize += sizeof(double);

	return currentEntryTrailIndex - 1;
}

void DataBuffer::GetData(uint32_t entryIndex, float& value) const
{
	value = 0.0f;
	if (entryIndex >= currentEntryTrailIndex)
		return;

	const Entry& entry = entries[entryIndex];
	const double* value_data = reinterpret_cast<const double*>(&data[entry.offset]);

	value = static_cast<float>(*value_data);
}

uint32_t DataBuffer::GetLastEntryIndex() const
{
	return currentEntryTrailIndex - 1;
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
		for (const auto& shaderStage : pass.shaderStages)
		{
			std::cout << shaderStage.stage << "---" << shaderStage.code->name << std::endl;
			std::cout << "shader_stage.code->code: " << shaderStage.code->code << std::endl;
		}
	}
	for (const auto& property : properties)
	{
		std::cout << "Property: " << property->name << std::endl;
		std::cout << "UI Name: " << property->uiName << std::endl;
		std::cout << "Default Value: " << property->defaultValue << std::endl;
		std::cout << "Type: " << property->type << std::endl;
	}
}

Lexer::Lexer(const std::string& source, DataBuffer& inDataBuffer): position(source.c_str()), line(1), column(0), hasError(false), errorLine(1),
	dataBuffer(inDataBuffer) {}

Lexer::Lexer(const Lexer& other): position(other.position), line(other.line), column(other.column), hasError(other.hasError),
	errorLine(other.errorLine), dataBuffer(other.dataBuffer) {}

Lexer& Lexer::operator=(const Lexer& other)
{
	position = other.position;
	line = other.line;
	column = other.column;
	hasError = other.hasError;
	errorLine = other.errorLine;
	dataBuffer = other.dataBuffer;
	return *this;
}

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
	{'"', TokenType::Token_String},
};

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

bool Lexer::CheckToken(const Token& token, TokenType expected_type)
{
	if (hasError)
		return true;

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
	int32_t decimalPart = 0;
	if (*position > '0' && *position <= '9')
	{
		decimalPart = (*position - '0');
		++position;

		while (*position != '.' && IsNumber(*position))
		{
			decimalPart = (decimalPart * 10) + (*position - '0');

			++position;
		}
	}
	return decimalPart;
}

void Lexer::HandleFractionalPart(int32_t& fractionalPart, int32_t& fractionalDivisor)
{
	if (*position == '.')
	{
		++position;

		while (IsNumber(*position))
		{
			fractionalPart = (fractionalPart * 10) + (*position - '0');
			fractionalDivisor *= 10;

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
	int32_t decimalPart = HandleDecimalPart();
	int32_t fractionalPart = 0;
	int32_t fractionalDivisor = 1;
	HandleFractionalPart(fractionalPart, fractionalDivisor);
	HandleExponent();
	double parsed_number = (double)sign * (decimalPart + ((double)fractionalPart / fractionalDivisor));
	dataBuffer.AddData(parsed_number);
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

bool Parser::TryParseIfDefined(const Token& token, CodeFragment& codeFragment)
{
	if (ExpectKeyword(token.text, "if"))
	{
		Token newToken;
		lexer.NextToken(newToken);

		if (ExpectKeyword(newToken.text, "defined"))
		{
			lexer.NextToken(newToken);
			++codeFragment.ifdefDepth;

			if (ExpectKeyword(newToken.text, "VERTEX"))
			{
				codeFragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Vertex)] = codeFragment.ifdefDepth;
				codeFragment.currentStage = ShaderStage::Vertex;
			}
			else if (ExpectKeyword(newToken.text, "FRAGMENT"))
			{
				codeFragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Fragment)] = codeFragment.ifdefDepth;
				codeFragment.currentStage = ShaderStage::Fragment;
			}
			else if (ExpectKeyword(newToken.text, "COMPUTE"))
			{
				codeFragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Compute)] = codeFragment.ifdefDepth;
				codeFragment.currentStage = ShaderStage::Compute;
			}
		}

		return true;
	}
	return false;
}

bool Parser::TryParsePragma(const Token& token, CodeFragment& codeFragment)
{
	if (ExpectKeyword(token.text, "pragma"))
	{
		Token newToken;
		lexer.NextToken(newToken);

		if (ExpectKeyword(newToken.text, "include"))
		{
			lexer.NextToken(newToken);

			codeFragment.includes.emplace_back(newToken.text);
			codeFragment.includesFlags.emplace_back((uint32_t)codeFragment.currentStage);
		}
		else if (ExpectKeyword(newToken.text, "include_hfx"))
		{
			lexer.NextToken(newToken);

			codeFragment.includes.emplace_back(newToken.text);
			uint32_t flag = (uint32_t)codeFragment.currentStage | 0x10; // 0x10 = local hfx.
			codeFragment.includesFlags.emplace_back(flag);
		}

		return true;
	}
	return false;
}

bool Parser::TryParseEndIf(const Token& token, CodeFragment& codeFragment)
{
	if (ExpectKeyword(token.text, "endif"))
	{
		if (codeFragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Vertex)] == codeFragment.ifdefDepth)
		{
			codeFragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Vertex)] = 0xffffffff;
			codeFragment.currentStage = ShaderStage::Count;
		}
		else if (codeFragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Fragment)] == codeFragment.ifdefDepth)
		{
			codeFragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Fragment)] = 0xffffffff;
			codeFragment.currentStage = ShaderStage::Count;
		}
		else if (codeFragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Compute)] == codeFragment.ifdefDepth)
		{
			codeFragment.stageIfdefDepth[static_cast<uint32_t>(ShaderStage::Compute)] = 0xffffffff;
			codeFragment.currentStage = ShaderStage::Count;
		}

		--codeFragment.ifdefDepth;

		return true;
	}
	return false;
}

void Parser::DirectiveIdentifier(const Token& token, CodeFragment& codeFragment)
{
	for (uint32_t i = 0; i < token.text.length; ++i)
	{
		char c = *(token.text.text + i);

		switch (c)
		{
			case 'i':
			{
				if (TryParseIfDefined(token, codeFragment))
					return;
				break;
			}

			case 'p':
			{
				if (TryParsePragma(token, codeFragment))
					return;
				break;
			}

			case 'e':
			{
				if (TryParseEndIf(token, codeFragment))
					return;
				break;
			}
		}
	}
}

void Parser::UniformIdentifier(const Token& token, CodeFragment& codeFragment)
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
					codeFragment.resources.emplace_back(resource);
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
					codeFragment.resources.emplace_back(resource);
				}
				break;
			}
		}
	}
}

void Parser::ParseGlslContent(Token& token, CodeFragment codeFragment)
{
	uint32_t openBraces = 1;

	// Scan until close brace token
	while (openBraces)
	{
		if (token.type == TokenType::Token_OpenBrace)
			++openBraces;
		else if (token.type == TokenType::Token_CloseBrace)
			--openBraces;

		// Parse hash for includes and defines
		if (token.type == TokenType::Token_Hash)
		{
			// Get next token and check which directive is
			lexer.NextToken(token);

			DirectiveIdentifier(token, codeFragment);
		}
		else if (token.type == TokenType::Token_Identifier)
		{
			// Parse uniforms to add resource dependencies if not explicit in the HFX file.
			if (ExpectKeyword(token.text, "uniform"))
			{
				lexer.NextToken(token);

				UniformIdentifier(token, codeFragment);
			}
		}

		// Only advance token when we are inside the glsl braces, otherwise will skip the following glsl part.
		if (openBraces)
			lexer.NextToken(token);
	}
}

void Parser::DeclarationGlsl()
{
	Token token;
	if (!lexer.ExpectToken(token, TokenType::Token_Identifier)) { return; }
	CodeFragment codeFragment = {};
	codeFragment.name = token.text;

	for (size_t i = 0; i < static_cast<uint32_t>(ShaderStage::Count); i++) { codeFragment.stageIfdefDepth[i] = 0xffffffff; }

	if (!lexer.ExpectToken(token, TokenType::Token_OpenBrace)) { return; }

	lexer.NextToken(token);
	codeFragment.code.text = token.text.text;

	ParseGlslContent(token, codeFragment);
	codeFragment.code.length = token.text.text - codeFragment.code.text;

	ast.codeFragments.emplace_back(codeFragment);
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
					pass.shaderStages.emplace_back(stage);
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
					pass.shaderStages.emplace_back(stage);
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
					pass.shaderStages.emplace_back(stage);
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

void Parser::DeclarationProperties()
{
	Token token;

	if (!lexer.ExpectToken(token, TokenType::Token_OpenBrace)) { return; }

	uint32_t open_braces = 1;
	lexer.NextToken(token);

	while (open_braces)
	{
		if (token.type == TokenType::Token_OpenBrace)
			++open_braces;
		else if (token.type == TokenType::Token_CloseBrace)
			--open_braces;

		if (token.type == TokenType::Token_Identifier) { DeclarationProperty(token.text); }

		if (open_braces)
			lexer.NextToken(token);
	}
}

bool Parser::NumberAndIdentifier(Token& token)
{
	lexer.NextToken(token);
	if (token.type == TokenType::Token_Number)
	{
		Token numberToken = token;
		lexer.NextToken(token);

		// Extend current token to include the number.
		token.text.text = numberToken.text.text; //TODO
		token.text.length += numberToken.text.length;
	}

	if (token.type != TokenType::Token_Identifier) { return false; }
	return true;
}

void Parser::ParsePropertyDefaultValue(Property* property, Token token)
{
	Lexer CachedLexer = CacheLexer(lexer);

	lexer.NextToken(token);
	// At this point only the optional default value is missing, otherwise the parsing is over.
	if (token.type == TokenType::Token_Equals)
	{
		lexer.NextToken(token);

		if (token.type == TokenType::Token_Number)
		{
			// Cache the data buffer entry index into the property for later retrieval.
			property->dataIndex = dataBuffer.GetLastEntryIndex();
		}
		else if (token.type == TokenType::Token_OpenParen)
		{
			// TODO: Colors and Vectors
			// (number0, number1, ...)
		}
		else if (token.type == TokenType::Token_String)
		{
			// For Texture.
			property->defaultValue = token.text;
		}
		else
		{
			throw std::runtime_error("Invalid default value for property.");
		}
	}
	else { lexer = CachedLexer; }
}

void Parser::DeclarationProperty(const IndirecString& name)
{
	Property* property = new Property();
	property->name = name;

	Token token;
	if (!lexer.ExpectToken(token, TokenType::Token_OpenParen)) { return; }
	if (!lexer.ExpectToken(token, TokenType::Token_String)) { return; }
	property->uiName = token.text;
	if (!lexer.ExpectToken(token, TokenType::Token_Comma)) { return; }

	// Handle property type like '2D', 'Float'
	if (!NumberAndIdentifier(token)) { return; }
	property->type = PropertyTypeIdentifier(token);

	lexer.NextToken(token);
	if (!lexer.CheckToken(token, TokenType::Token_CloseParen)) { return; }
	ParsePropertyDefaultValue(property, token);

	ast.properties.push_back(property);
}

PropertyType Parser::PropertyTypeIdentifier(const Token& token)
{
	for (uint32_t i = 0; i < token.text.length; ++i)
	{
		char c = *(token.text.text + i);
		switch (c)
		{
			case '1':
			{
				if (ExpectKeyword(token.text, "1D")) { return PropertyType::Texture1D; }
				break;
			}
			case '2':
			{
				if (ExpectKeyword(token.text, "2D")) { return PropertyType::Texture2D; }
				break;
			}
			case '3':
			{
				if (ExpectKeyword(token.text, "3D")) { return PropertyType::Texture3D; }
				break;
			}
			case 'V':
			{
				if (ExpectKeyword(token.text, "Volume")) { return PropertyType::TextureVolume; }
				else
					if (ExpectKeyword(token.text, "Vector")) { return PropertyType::Vector; }
				break;
			}
			case 'I':
			{
				if (ExpectKeyword(token.text, "Int")) { return PropertyType::Int; }
				break;
			}
			case 'R':
			{
				if (ExpectKeyword(token.text, "Range")) { return PropertyType::Range; }
				break;
			}
			case 'F':
			{
				if (ExpectKeyword(token.text, "Float")) { return PropertyType::Float; }
				break;
			}
			case 'C':
			{
				if (ExpectKeyword(token.text, "Color")) { return PropertyType::Color; }
				break;
			}
			default:
			{
				return PropertyType::Unknown;
				break;
			}
		}
	}

	return PropertyType::Unknown;
}

Lexer Parser::CacheLexer(const Lexer& lexer) { return Lexer(lexer); }

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
				else if (ExpectKeyword(token.text, "properties"))
				{
					DeclarationProperties();
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

ShaderGenerator::ShaderGenerator(const AST& ast): ast(ast) {}

void ShaderGenerator::GenerateShaders(const std::string& path)
{
	const uint32_t passCount = (uint32_t)ast.passes.size();
	for (uint32_t i = 0; i < passCount; i++)
	{
		const Pass& pass = ast.passes[i];
		for (size_t s = 0; s < pass.shaderStages.size(); ++s) { OutputShaderStage(path, pass.shaderStages[s]); }
	}
}

void ShaderGenerator::OutputShaderStage(const std::string& path, const Pass::Stage& stage)
{
	const CodeFragment* codeFragment = stage.code;
	if (codeFragment == nullptr)
		return;

	std::string fileName = path + codeFragment->name.ToString() + "_" + ShaderStage2Postfix(stage.stage) + ".glsl";
	std::ofstream file(fileName);
	std::string code = codeFragment->code.ToString();
	file << "#version 330 core\n";
	file << "#define " << ShaderStage2String(stage.stage) << "\n";
	file << code;
	file.close();
}

void CompileShaderEffectFile(AST& ast, const DataBuffer& dataBuffer)
{
	// Calculate Output File Path
	std::string outputFilePath = ""; //TODO
	StringBuffer outDefaults;
	StringBuffer outBuffer;

	// For each property, generate glsl code, get default value, (handle alignment)
	if (!ast.properties.size())
	{
		uint32_t zeroSize = 0;
		outDefaults.AppendMemory(&zeroSize, sizeof(uint32_t));
		return;
	}

	// Add the local constants into the code.
	outBuffer.AppendFormat("\n\t\tlayout (std140, binding=7) uniform LocalConstants {\n\n");

	// For GPU the struct must be 16 bytes aligned. Track alignment
	uint32_t gpuStructAlignment = 0;

	// In the defaults, write the type, size in '4 bytes' blocks, then data.
	ResourceType resourceType = ResourceType::Constants;
	outDefaults.AppendMemory(&resourceType, sizeof(ResourceType));

	// Reserve space for later writing the correct value.
	char* bufferSizeMemory = outDefaults.Allocate(sizeof(uint32_t));

	for (size_t i = 0; i < ast.properties.size(); i++)
	{
		Property* property = ast.properties[i];

		switch (property->type)
		{
			case PropertyType::Float:
			{
				outBuffer.AppendFormat("\t\t\tfloat\t\t\t\t\t");
				outBuffer.AppendIndirectString(property->name);
				outBuffer.AppendFormat(";\n");

				// Get default value and write it into default buffer
				if (property->dataIndex != INVALID_PROPERTY_DATA_INDEX)
				{
					float value = 0.0f;
					dataBuffer.GetData(property->dataIndex, value);
					outDefaults.AppendMemory(&value, sizeof(float));
				}
				// Update offset
				property->offsetInBytes = gpuStructAlignment * 4;

				++gpuStructAlignment;
				break;
			}

			case PropertyType::Int: { break; }

			case PropertyType::Range: { break; }

			case PropertyType::Color: { break; }

			case PropertyType::Vector: { break; }
		}
	}

	uint32_t tailPaddingSize = 4 - (gpuStructAlignment % 4);
	outBuffer.AppendFormat("\t\t\tfloat\t\t\t\t\tpad_tail[%u];\n\n", tailPaddingSize);
	outBuffer.AppendFormat("\t\t} local_constants;\n\n");

	for (uint32_t v = 0; v < tailPaddingSize; ++v)
	{
		float value = 0.0f;
		outDefaults.AppendMemory(&value, sizeof(float));
	}

	// Write the constant buffer size in bytes.
	uint32_t constantsBufferSize = (gpuStructAlignment + tailPaddingSize) * sizeof(float);
	memcpy(bufferSizeMemory, &constantsBufferSize, sizeof(uint32_t));
}

void CompileHFX(const std::string& filePath)
{
	std::string content = FileReader(filePath).Read();
	DataBuffer dataBuffer(256, 2048);
	Lexer lexer(content, dataBuffer);
	Parser parser(lexer, dataBuffer);
	parser.GenerateAST();
	AST& ast = parser.GetAST();
	ast.Print();
	dataBuffer.Print();
	ShaderGenerator shaderGenerator(ast);
	shaderGenerator.GenerateShaders(ST::PathManager::GetHFXDir());

	CompileShaderEffectFile(ast, dataBuffer);
}
}
