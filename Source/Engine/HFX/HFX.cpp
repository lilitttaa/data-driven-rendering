#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>
#include "HFX.h"
#include <cstdarg>
#include "PathManager.h"
#include "File/FileReader.h"
#include "Serlalizer/Serializer.h"

namespace HFX
{
std::string kShaderTypeTable[] = {"VERTEX", "FRAGMENT", "GEOMETRY", "COMPUTE", "HULL", "DOMAIN"};
std::string kShaderTypePostfixTable[] = {"vt", "fg", "gm", "cp", "hl", "dm"};

std::string ShaderType2Postfix(graphics::ShaderType stage) { return kShaderTypePostfixTable[static_cast<uint32_t>(stage)]; }

std::string ShaderType2String(graphics::ShaderType stage) { return kShaderTypeTable[static_cast<uint32_t>(stage)]; }

std::ostream& operator<<(std::ostream& os, const graphics::ShaderType& type)
{
	switch (type)
	{
		case graphics::ShaderType::kVertex: { os << "Vertex"; }
		break;
		case graphics::ShaderType::kFragment: { os << "Fragment"; }
		break;
		case graphics::ShaderType::kGeometry: { os << "Geometry"; }
		break;
		case graphics::ShaderType::kCompute: { os << "Compute"; }
		break;
		case graphics::ShaderType::kHull: { os << "Hull"; }
		break;
		case graphics::ShaderType::kDomain: { os << "Domain"; }
		break;
		case graphics::ShaderType::kCount: { os << "Count"; }
		break;
	}
	return os;
}

bool ExpectKeyword(const IndirectString& text, const std::string& expected_keyword)
{
	if (text.length_ != expected_keyword.length())
		return false;

	for (uint32_t i = 0; i < expected_keyword.length(); ++i)
	{
		if (text.text_[i] != expected_keyword[i])
			return false;
	}
	return true;
}

bool IndirectString::Equals(const IndirectString& a, const IndirectString& b)
{
	if (a.length_ != b.length_)
		return false;
	for (size_t i = 0; i < a.length_; ++i)
	{
		if (a.text_[i] != b.text_[i])
			return false;
	}
	return true;
}

std::ostream& operator<<(std::ostream& os, const IndirectString& str)
{
	for (size_t i = 0; i < str.length_; ++i) { os << str.text_[i]; }
	return os;
}

void StringBuffer::Reset(uint32_t size)
{
	data_.reserve(size);
	data_.clear();
}

void StringBuffer::Clear() { data_.clear(); }

#define STRING_BUFFER_RESERVE_SIZE 1024

void StringBuffer::AppendFormat(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::vector<char> temp;
	temp.resize(STRING_BUFFER_RESERVE_SIZE);
	while (true)
	{
		va_list args_copy;
		args_copy = args;
		int written_chars = vsnprintf(temp.data(), temp.size(), format, args_copy);
		if (written_chars >= 0 && written_chars < static_cast<int>(temp.size()))
		{
			data_.insert(data_.end(), temp.begin(), temp.begin() + written_chars);
			break;
		}
		else { temp.resize(temp.size() * 2); }
	}
	va_end(args);
}

void StringBuffer::AppendIndirectString(const IndirectString& text)
{
	if (text.length_ > 0) { data_.insert(data_.end(), text.text_, text.text_ + text.length_); }
}

void StringBuffer::AppendMemory(void* memory, uint32_t size)
{
	if (size > 0) { data_.insert(data_.end(), static_cast<char*>(memory), static_cast<char*>(memory) + size); }
}

void StringBuffer::AppendStringBuffer(const StringBuffer& other_buffer)
{
	data_.insert(data_.end(), other_buffer.data_.begin(), other_buffer.data_.end());
}

char* StringBuffer::Allocate(uint32_t size)
{
	if (data_.capacity() - data_.size() < size) { data_.reserve(data_.capacity() + size); }
	size_t offset = data_.size();
	data_.resize(data_.size() + size);
	return data_.data() + offset;
}

size_t StringBuffer::Size() const { return data_.size(); }
const char* StringBuffer::CStr() const { return data_.data(); }

DataBuffer::DataBuffer(uint32_t max_entries, uint32_t buffer_size): entries_(max_entries), data_(buffer_size, '\0'),
	max_entries_(max_entries), current_entry_trail_index_(0),
	buffer_size_(buffer_size), current_size_(0) {}

void DataBuffer::Reset()
{
	current_size_ = 0;
	current_entry_trail_index_ = 0;
}

uint32_t DataBuffer::AddData(double in_data)
{
	if (current_entry_trail_index_ >= max_entries_)
		return -1;

	if (current_size_ + sizeof(double) >= buffer_size_)
		return -1;

	// Init entry
	Entry& entry = entries_[current_entry_trail_index_++];
	entry.offset = current_size_;

	// Copy data
	std::memcpy(&data_[current_size_], &in_data, sizeof(double));
	current_size_ += sizeof(double);

	return current_entry_trail_index_ - 1;
}

void DataBuffer::GetData(uint32_t entry_index, float& value) const
{
	value = 0.0f;
	if (entry_index >= current_entry_trail_index_)
		return;

	const Entry& entry = entries_[entry_index];
	const double* value_data = reinterpret_cast<const double*>(&data_[entry.offset]);

	value = static_cast<float>(*value_data);
}

void DataBuffer::GetData(float& value) const { GetData(GetLastEntryIndex(), value); }

uint32_t DataBuffer::GetLastEntryIndex() const { return current_entry_trail_index_ - 1; }

// void AST::Print()
// {
// 	std::cout << "Shader: " << name_ << std::endl;
// 	for (const auto& code_fragment : code_fragments_)
// 	{
// 		std::cout << "CodeFragment: " << code_fragment.name_ << std::endl;
// 		std::cout << "Code: " << code_fragment.code_ << std::endl;
// 		std::cout << "Ifdef Depth: " << code_fragment.if_def_depth_ << std::endl;
// 		std::cout << "Current Stage: " << static_cast<uint32_t>(code_fragment.current_stage_) << std::endl;
// 		for (const auto& resource : code_fragment.resources_) { std::cout << "Resource: " << resource.name_ << std::endl; }
// 		for (const auto& include : code_fragment.includes_) { std::cout << "Include: " << include << std::endl; }
// 		for (const auto& include_flag : code_fragment.includes_flags_) { std::cout << "Include Flag: " << include_flag << std::endl; }
// 		for (size_t i = 0; i < static_cast<uint32_t>(ShaderStage::kCount); i++)
// 		{
// 			std::cout << "Stage Ifdef Depth: " << code_fragment.stage_if_def_depth_[i] << std::endl;
// 		}
// 	}
// 	for (const auto& pass : passes_)
// 	{
// 		std::cout << "Pass: " << pass.name_ << std::endl;
// 		for (const auto& shader_stage : pass.shader_stages_)
// 		{
// 			std::cout << shader_stage.stage << "---" << shader_stage.code->name_ << std::endl;
// 			std::cout << "shader_stage.code->code: " << shader_stage.code->code_ << std::endl;
// 		}
// 	}
// 	for (const auto& property : properties_)
// 	{
// 		std::cout << "Property: " << property->name_ << std::endl;
// 		std::cout << "UI Name: " << property->ui_name_ << std::endl;
// 		std::cout << "Default Value: " << property->default_value_ << std::endl;
// 		std::cout << "Type: " << property->type_ << std::endl;
// 	}
// }

std::ostream& operator<<(std::ostream& os, const ShaderEffect& shader_effect)
{
	os << "Shader Effect: " << shader_effect.name_ << std::endl;
	return os;
}

BinarySerializer& operator<<(BinarySerializer& serializer, ShaderEffect& shader_effect)
{
	serializer << shader_effect.name_;
	serializer << shader_effect.passes_;
	serializer << shader_effect.code_chunks_;
	serializer << shader_effect.resource_lists_;
	serializer << shader_effect.render_states_;
	// uint32_t properties_size = 0;
	// if (serializer.GetAction() == SerializerAction::kWrite) { properties_size = static_cast<uint32_t>(shader_effect.properties_.size()); }
	// serializer << properties_size;
	// if (serializer.GetAction() == SerializerAction::kRead) { shader_effect.properties_.resize(properties_size); }
	// for (auto& property : shader_effect.properties_) { serializer << *property; }
	return serializer;
}

Lexer::Lexer(const std::string& source, DataBuffer& in_data_buffer): position_(source.c_str()), line_(1), column_(0), has_error_(false),
	error_line_(1),
	data_buffer_(in_data_buffer) {}

Lexer::Lexer(const Lexer& other): position_(other.position_), line_(other.line_), column_(other.column_), has_error_(other.has_error_),
	error_line_(other.error_line_), data_buffer_(other.data_buffer_) {}

Lexer& Lexer::operator=(const Lexer& other)
{
	position_ = other.position_;
	line_ = other.line_;
	column_ = other.column_;
	has_error_ = other.has_error_;
	error_line_ = other.error_line_;
	data_buffer_ = other.data_buffer_;
	return *this;
}

void Lexer::GetTokenTextFromString(IndirectString& token_text)
{
	token_text.text_ = position_;
	while (position_[0] &&
	       position_[0] != '"')
	{
		if ((position_[0] == '\\') &&
		    position_[1]) { ++position_; }
		++position_;
	}
	token_text.length_ = static_cast<uint32_t>(position_ - token_text.text_);
	if (position_[0] == '"') { ++position_; }
}

bool Lexer::IsIdOrKeyword(char c) { return IsAlpha(c); }

std::map<char, TokenType> tokenMap = {
	{'\0', TokenType::kToken_EndOfStream},
	{'(', TokenType::kToken_OpenParen},
	{')', TokenType::kToken_CloseParen},
	{':', TokenType::kToken_Colon},
	{';', TokenType::kToken_Semicolon},
	{'*', TokenType::kToken_Asterisk},
	{'[', TokenType::kToken_OpenBracket},
	{']', TokenType::kToken_CloseBracket},
	{'{', TokenType::kToken_OpenBrace},
	{'}', TokenType::kToken_CloseBrace},
	{'=', TokenType::kToken_Equals},
	{'#', TokenType::kToken_Hash},
	{',', TokenType::kToken_Comma},
	{'"', TokenType::kToken_String},
};

void Lexer::NextToken(Token& token)
{
	SkipWhitespaceAndComments();
	token.Init(position_, line_);
	char c = position_[0];
	++position_;
	if (tokenMap.find(c) != tokenMap.end())
	{
		token.type_ = tokenMap[c];
		if (token.type_ == TokenType::kToken_String) { GetTokenTextFromString(token.text_); }
	}
	else
	{
		if (IsIdOrKeyword(c))
		{
			token.type_ = TokenType::kToken_Identifier;
			while (IsAlpha(position_[0]) || IsNumber(position_[0]) || (position_[0] == '_')) { ++position_; }
			token.text_.length_ = static_cast<uint32_t>(position_ - token.text_.text_);
		}
		else if (IsNumber(c) || c == '-')
		{
			token.type_ = TokenType::kToken_Number;
			// Backtrack to start properly parsing the number
			--position_;
			ParseNumber();
			token.text_.length_ = static_cast<uint32_t>(position_ - token.text_.text_);
		}
		else { token.type_ = TokenType::kToken_Unknown; }
	}
}

bool Lexer::EqualToken(Token& token, TokenType expected_type)
{
	NextToken(token);
	return token.type_ == expected_type;
}

bool Lexer::ExpectToken(Token& token, TokenType expected_type)
{
	if (has_error_)
		return true;

	NextToken(token);
	has_error_ = token.type_ != expected_type;
	if (has_error_)
	{
		// Save line of error
		error_line_ = line_;
	}
	return !has_error_;
}

bool Lexer::CheckToken(const Token& token, TokenType expected_type)
{
	if (has_error_)
		return true;

	has_error_ = token.type_ != expected_type;

	if (has_error_)
	{
		// Save line of error
		error_line_ = line_;
	}
	return !has_error_;
}

bool Lexer::IsEndOfLine(char c) { return (c == '\n' || c == '\r'); }

bool Lexer::IsWhitespace(char c) { return (c == ' ' || c == '\t' || IsEndOfLine(c)); }

bool Lexer::IsAlpha(char c) { return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')); }

bool Lexer::IsNumber(char c) { return (c >= '0' && c <= '9'); }

bool Lexer::IsSingleLineComments() { return (position_[0] == '/') && (position_[1] == '/'); }

void Lexer::SkipComments()
{
	position_ += 2;
	while (position_[0] && !IsEndOfLine(position_[0])) { ++position_; }
}

bool Lexer::IsCStyleComments() { return (position_[0] == '/') && (position_[1] == '*'); }

void Lexer::SkipCStyleComments()
{
	position_ += 2;

	while (!((position_[0] == '*') && (position_[1] == '/')))
	{
		if (IsEndOfLine(position_[0]))
			++line_;

		++position_;
	}

	if (position_[0] == '*') { position_ += 2; }
}

void Lexer::SkipWhitespaceAndComments()
{
	while (true)
	{
		if (IsWhitespace(position_[0]))
		{
			if (IsEndOfLine(position_[0]))
				++line_;

			++position_;
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
		++position_;
	}
	return sign;
}

void Lexer::HeadingZero()
{
	if (*position_ == '0')
	{
		++position_;

		while (*position_ == '0')
			++position_;
	}
}

int32_t Lexer::HandleDecimalPart()
{
	int32_t decimal_part = 0;
	if (*position_ > '0' && *position_ <= '9')
	{
		decimal_part = (*position_ - '0');
		++position_;

		while (*position_ != '.' && IsNumber(*position_))
		{
			decimal_part = (decimal_part * 10) + (*position_ - '0');

			++position_;
		}
	}
	return decimal_part;
}

void Lexer::HandleFractionalPart(int32_t& fractional_part, int32_t& fractional_divisor)
{
	if (*position_ == '.')
	{
		++position_;

		while (IsNumber(*position_))
		{
			fractional_part = (fractional_part * 10) + (*position_ - '0');
			fractional_divisor *= 10;

			++position_;
		}
	}
}

void Lexer::HandleExponent() { if (*position_ == 'e' || *position_ == 'E') { ++position_; } }
// Parse the following literals:
// 58, -58, 0.003, 4e2, 123.456e-67, 0.1E4f
void Lexer::ParseNumber()
{
	int32_t sign = CheckSign(position_[0]);
	// 00.003
	HeadingZero();
	int32_t decimal_part = HandleDecimalPart();
	int32_t fractional_part = 0;
	int32_t fractional_divisor = 1;
	HandleFractionalPart(fractional_part, fractional_divisor);
	HandleExponent();
	double parsed_number = (double)sign * (decimal_part + ((double)fractional_part / fractional_divisor));
	data_buffer_.AddData(parsed_number);
}

void Parser::Parse()
{
	bool parsing = true;
	while (parsing)
	{
		Token token;
		lexer.NextToken(token);
		switch (token.type_)
		{
			case TokenType::kToken_Identifier:
			{
				Identifier(token);
				break;
			}

			case TokenType::kToken_EndOfStream:
			{
				parsing = false;
				break;
			}
			default: break;
		}
	}
}

void Parser::DeclarationEffect()
{
	Token token;
	if (!lexer.ExpectToken(token, TokenType::kToken_Identifier)) { return; }

	// ast_.name_ = token.text_;
	shader_effect_.name_ = token.text_.ToString();

	if (!lexer.ExpectToken(token, TokenType::kToken_OpenBrace)) { return; }

	while (!lexer.EqualToken(token, TokenType::kToken_CloseBrace)) { Identifier(token); }
}

bool Parser::TryParseIfDefined(const Token& token, CodeChunk& code_chunk)
{
	if (ExpectKeyword(token.text_, "if"))
	{
		Token new_token;
		lexer.NextToken(new_token);

		if (ExpectKeyword(new_token.text_, "defined"))
		{
			lexer.NextToken(new_token);
			// ++code_chunk.if_def_depth_;

			if (ExpectKeyword(new_token.text_, "VERTEX"))
			{
				// code_chunk.stage_if_def_depth_[static_cast<uint32_t>(ShaderStage::kVertex)] = code_chunk.if_def_depth_;
				// code_chunk.current_stage_ = ShaderStage::kVertex;
			}
			else if (ExpectKeyword(new_token.text_, "FRAGMENT"))
			{
				// code_chunk.stage_if_def_depth_[static_cast<uint32_t>(ShaderStage::kFragment)] = code_chunk.if_def_depth_;
				// code_chunk.current_stage_ = ShaderStage::kFragment;
			}
			else if (ExpectKeyword(new_token.text_, "COMPUTE"))
			{
				// code_chunk.stage_if_def_depth_[static_cast<uint32_t>(ShaderStage::kCompute)] = code_chunk.if_def_depth_;
				// code_chunk.current_stage_ = ShaderStage::kCompute;
			}
		}

		return true;
	}
	return false;
}

bool Parser::TryParsePragma(const Token& token, CodeChunk& code_chunk)
{
	if (ExpectKeyword(token.text_, "pragma"))
	{
		Token new_token;
		lexer.NextToken(new_token);

		if (ExpectKeyword(new_token.text_, "include"))
		{
			lexer.NextToken(new_token);

			code_chunk.includes_.emplace_back(new_token.text_.ToString());
			// code_chunk.includes_flags_.emplace_back((uint32_t)code_chunk.current_stage_);
		}
		else if (ExpectKeyword(new_token.text_, "include_hfx"))
		{
			lexer.NextToken(new_token);

			code_chunk.includes_.emplace_back(new_token.text_.ToString());
			// uint32_t flag = (uint32_t)code_chunk.current_stage_ | 0x10; // 0x10 = local hfx.
			// code_chunk.includes_flags_.emplace_back(flag);
		}

		return true;
	}
	return false;
}

bool Parser::TryParseEndIf(const Token& token, CodeChunk& code_chunk)
{
	if (ExpectKeyword(token.text_, "endif"))
	{
		// if (code_chunk.stage_if_def_depth_[static_cast<uint32_t>(ShaderStage::kVertex)] == code_chunk.if_def_depth_)
		// {
		// 	code_chunk.stage_if_def_depth_[static_cast<uint32_t>(ShaderStage::kVertex)] = 0xffffffff;
		// 	code_chunk.current_stage_ = ShaderStage::kCount;
		// }
		// else if (code_chunk.stage_if_def_depth_[static_cast<uint32_t>(ShaderStage::kFragment)] == code_chunk.if_def_depth_)
		// {
		// 	code_chunk.stage_if_def_depth_[static_cast<uint32_t>(ShaderStage::kFragment)] = 0xffffffff;
		// 	code_chunk.current_stage_ = ShaderStage::kCount;
		// }
		// else if (code_chunk.stage_if_def_depth_[static_cast<uint32_t>(ShaderStage::kCompute)] == code_chunk.if_def_depth_)
		// {
		// 	code_chunk.stage_if_def_depth_[static_cast<uint32_t>(ShaderStage::kCompute)] = 0xffffffff;
		// 	code_chunk.current_stage_ = ShaderStage::kCount;
		// }
		//
		// --code_chunk.if_def_depth_;

		return true;
	}
	return false;
}

void Parser::DirectiveIdentifier(const Token& token, CodeChunk& code_chunk)
{
	for (uint32_t i = 0; i < token.text_.length_; ++i)
	{
		char c = *(token.text_.text_ + i);

		switch (c)
		{
			case 'i':
			{
				if (TryParseIfDefined(token, code_chunk))
					return;
				break;
			}

			case 'p':
			{
				if (TryParsePragma(token, code_chunk))
					return;
				break;
			}

			case 'e':
			{
				if (TryParseEndIf(token, code_chunk))
					return;
				break;
			}
		}
	}
}

void Parser::UniformIdentifier(const Token& token, CodeChunk& code_chunk)
{
	for (uint32_t i = 0; i < token.text_.length_; ++i)
	{
		char c = *(token.text_.text_ + i);

		switch (c)
		{
			case 'i':
			{
				if (ExpectKeyword(token.text_, "image2D"))
				{
					// Advance to next token to get the name
					Token name_token;
					lexer.NextToken(name_token);

					Resource resource = {graphics::ResourceType::kTextureRW, name_token.text_.ToString()};
					code_chunk.resources_.emplace_back(resource);
				}
				break;
			}

			case 's':
			{
				if (ExpectKeyword(token.text_, "sampler2D"))
				{
					// Advance to next token to get the name
					Token name_token;
					lexer.NextToken(name_token);

					Resource resource = {graphics::ResourceType::kTexture, name_token.text_.ToString()};
					code_chunk.resources_.emplace_back(resource);
				}
				break;
			}
		}
	}
}

void Parser::ParseGlslContent(Token& token, CodeChunk& code_chunk)
{
	uint32_t open_braces = 1;

	// Scan until close brace token
	while (open_braces)
	{
		if (token.type_ == TokenType::kToken_OpenBrace)
			++open_braces;
		else if (token.type_ == TokenType::kToken_CloseBrace)
			--open_braces;

		// Parse hash for includes and defines
		if (token.type_ == TokenType::kToken_Hash)
		{
			// Get next token and check which directive is
			lexer.NextToken(token);

			DirectiveIdentifier(token, code_chunk);
		}
		else if (token.type_ == TokenType::kToken_Identifier)
		{
			// Parse uniforms to add resource dependencies if not explicit in the HFX file.
			if (ExpectKeyword(token.text_, "uniform"))
			{
				lexer.NextToken(token);

				UniformIdentifier(token, code_chunk);
			}
		}

		// Only advance token when we are inside the glsl braces, otherwise will skip the following glsl part.
		if (open_braces)
			lexer.NextToken(token);
	}
}

void Parser::DeclarationGlsl()
{
	Token token;
	if (!lexer.ExpectToken(token, TokenType::kToken_Identifier)) { return; }
	// CodeFragment code_fragment = {};
	CodeChunk code_chunk = {};
	code_chunk.name_ = token.text_.ToString();
	// code_fragment.name_ = token.text_;

	// for (size_t i = 0; i < static_cast<uint32_t>(graphics::ShaderType::kCount); i++) { code_fragment.stage_if_def_depth_[i] = 0xffffffff; }

	if (!lexer.ExpectToken(token, TokenType::kToken_OpenBrace)) { return; }

	lexer.NextToken(token);
	IndirectString code = token.text_;
	// code_fragment.code_.text_ = token.text_.text_;

	ParseGlslContent(token, code_chunk);
	// code_fragment.code_.length_ = token.text_.text_ - code_fragment.code_.text_;
	code.length_ = token.text_.text_ - code.text_;
	code_chunk.code_ = code.ToString();

	shader_effect_.code_chunks_.emplace_back(code_chunk);
	// ast_.code_fragments_.emplace_back(code_fragment);
}

void Parser::DeclarationShader(Shader& out_shader)
{
	Token token;
	if (!lexer.ExpectToken(token, TokenType::kToken_Equals)) { return; }

	if (!lexer.ExpectToken(token, TokenType::kToken_Identifier)) { return; }

	out_shader.code_chunk_ref_ = FindCodeChunk(token.text_.ToString());
	// shader.code = FindCodeFragment(token.text_);
}

void Parser::PassIdentifier(const Token& token, Pass& pass)
{
	// Scan the name to know which stage we are parsing    
	for (uint32_t i = 0; i < token.text_.length_; ++i)
	{
		char c = *(token.text_.text_ + i);

		Shader shader = {};
		switch (c)
		{
			case 'c':
			{
				if (ExpectKeyword(token.text_, "compute"))
				{
					shader.type_ = graphics::ShaderType::kCompute;
					// Pass::Stage stage = {nullptr, ShaderStage::kCompute};
					DeclarationShader(shader);
					// pass.shader_stages_.emplace_back(stage);
					// return;
				}
				break;
			}

			case 'v':
			{
				if (ExpectKeyword(token.text_, "vertex"))
				{
					shader.type_ = graphics::ShaderType::kVertex;
					// Pass::Stage stage = {nullptr, ShaderStage::kVertex};
					DeclarationShader(shader);
					// pass.shader_stages_.emplace_back(stage);
					// return;
				}
				break;
			}

			case 'f':
			{
				if (ExpectKeyword(token.text_, "fragment"))
				{
					shader.type_ = graphics::ShaderType::kFragment;
					DeclarationShader(shader);
					// pass.shader_stages_.emplace_back(stage);
				}
				break;
			}
		}
		pass.shaders_.emplace_back(shader);
		return;
	}
}

void Parser::DeclarationPass()
{
	Token token;
	if (!lexer.ExpectToken(token, TokenType::kToken_Identifier)) { return; }

	Pass pass = {};
	// Cache name string
	// pass.name_ = token.text_;
	pass.name_ = token.text_.ToString();

	if (!lexer.ExpectToken(token, TokenType::kToken_OpenBrace)) { return; }
	while (!lexer.EqualToken(token, TokenType::kToken_CloseBrace)) { PassIdentifier(token, pass); }
	shader_effect_.passes_.emplace_back(pass);
	// ast_.passes_.emplace_back(pass);
}

void Parser::DeclarationProperties()
{
	Token token;

	if (!lexer.ExpectToken(token, TokenType::kToken_OpenBrace)) { return; }

	uint32_t open_braces = 1;
	lexer.NextToken(token);

	while (open_braces)
	{
		if (token.type_ == TokenType::kToken_OpenBrace)
			++open_braces;
		else if (token.type_ == TokenType::kToken_CloseBrace)
			--open_braces;

		if (token.type_ == TokenType::kToken_Identifier) { DeclarationProperty(token.text_); }

		if (open_braces)
			lexer.NextToken(token);
	}
}

bool Parser::NumberAndIdentifier(Token& token)
{
	lexer.NextToken(token);
	if (token.type_ == TokenType::kToken_Number)
	{
		Token number_token = token;
		lexer.NextToken(token);

		// Extend current token to include the number.
		token.text_.text_ = number_token.text_.text_; //TODO
		token.text_.length_ += number_token.text_.length_;
	}

	if (token.type_ != TokenType::kToken_Identifier) { return false; }
	return true;
}

void Parser::ParsePropertyDefaultValue(std::shared_ptr<Property> property, Token token)
{
	Lexer cached_lexer = CacheLexer(lexer);

	lexer.NextToken(token);
	// At this point only the optional default value is missing, otherwise the parsing is over.
	if (token.type_ == TokenType::kToken_Equals)
	{
		lexer.NextToken(token);

		if (token.type_ == TokenType::kToken_Number && property->type_ == graphics::PropertyType::kFloat)
		{
			// Cache the data buffer entry index into the property for later retrieval.
			float default_value = 0.0f;
			data_buffer.GetData(default_value);
			static_cast<FloatProperty*>(property.get())->default_value_ = default_value;
		}
		if (token.type_ == TokenType::kToken_Number && property->type_ == graphics::PropertyType::kInt)
		{
			// Cache the data buffer entry index into the property for later retrieval.
			float default_value = 0.0f;
			data_buffer.GetData(default_value);
			static_cast<IntProperty*>(property.get())->default_value_ = static_cast<int>(default_value);
		}
		else if (token.type_ == TokenType::kToken_OpenParen)
		{
			// TODO: Colors and Vectors
			// (number0, number1, ...)
		}
		else if (token.type_ == TokenType::kToken_String)
		{
			// For Texture.
			// property->default_value_ = token.text_;
		}
		else { throw std::runtime_error("Invalid default value for property."); }
	}
	else { lexer = cached_lexer; }
}

class PropertyFactory
{
public:
	static std::shared_ptr<Property> Create(graphics::PropertyType type)
	{
		switch (type)
		{
			case graphics::PropertyType::kInt: return std::make_shared<IntProperty>();
			case graphics::PropertyType::kFloat: return std::make_shared<FloatProperty>();
			case graphics::PropertyType::kRange: return std::make_shared<RangeProperty>();
			default: throw std::runtime_error("Invalid property type.");
		}
	}
};

void Parser::DeclarationProperty(const IndirectString& name)
{
	std::string property_name = name.ToString();

	Token token;
	if (!lexer.ExpectToken(token, TokenType::kToken_OpenParen)) { return; }
	if (!lexer.ExpectToken(token, TokenType::kToken_String)) { return; }
	std::string ui_name = token.text_.ToString();

	if (!lexer.ExpectToken(token, TokenType::kToken_Comma)) { return; }
	// Handle property type like '2D', 'Float'
	if (!NumberAndIdentifier(token)) { return; }
	graphics::PropertyType type = PropertyTypeIdentifier(token);

	std::shared_ptr<Property> property = PropertyFactory::Create(type);
	property->name_ = property_name;
	property->ui_name_ = ui_name;
	property->type_ = type;

	lexer.NextToken(token);
	if (!lexer.CheckToken(token, TokenType::kToken_CloseParen)) { return; }
	ParsePropertyDefaultValue(property, token);

	shader_effect_.properties_.push_back(property);
	// ast_.properties_.push_back(property);
}

graphics::PropertyType Parser::PropertyTypeIdentifier(const Token& token)
{
	for (uint32_t i = 0; i < token.text_.length_; ++i)
	{
		char c = *(token.text_.text_ + i);
		switch (c)
		{
			case '1':
			{
				if (ExpectKeyword(token.text_, "1D")) { return graphics::PropertyType::kTexture1D; }
				break;
			}
			case '2':
			{
				if (ExpectKeyword(token.text_, "2D")) { return graphics::PropertyType::kTexture2D; }
				break;
			}
			case '3':
			{
				if (ExpectKeyword(token.text_, "3D")) { return graphics::PropertyType::kTexture3D; }
				break;
			}
			case 'V':
			{
				if (ExpectKeyword(token.text_, "Volume")) { return graphics::PropertyType::kTextureVolume; }
				else
					if (ExpectKeyword(token.text_, "Vector")) { return graphics::PropertyType::kVector; }
				break;
			}
			case 'I':
			{
				if (ExpectKeyword(token.text_, "Int")) { return graphics::PropertyType::kInt; }
				break;
			}
			case 'R':
			{
				if (ExpectKeyword(token.text_, "Range")) { return graphics::PropertyType::kRange; }
				break;
			}
			case 'F':
			{
				if (ExpectKeyword(token.text_, "Float")) { return graphics::PropertyType::kFloat; }
				break;
			}
			case 'C':
			{
				if (ExpectKeyword(token.text_, "Color")) { return graphics::PropertyType::kColor; }
				break;
			}
			default:
			{
				return graphics::PropertyType::kUnknown;
				break;
			}
		}
	}

	return graphics::PropertyType::kUnknown;
}

Lexer Parser::CacheLexer(const Lexer& lexer) { return Lexer(lexer); }

void Parser::Identifier(const Token& token)
{
	for (uint32_t i = 0; i < token.text_.length_; ++i)
	{
		switch (token.text_.text_[i])
		{
			case 'e':
			{
				if (ExpectKeyword(token.text_, "effect"))
				{
					DeclarationEffect();
					return;
				}

				break;
			}

			case 'g':
			{
				if (ExpectKeyword(token.text_, "glsl"))
				{
					DeclarationGlsl();
					return;
				}
				break;
			}

			case 'p':
			{
				if (ExpectKeyword(token.text_, "pass"))
				{
					DeclarationPass();
					return;
				}
				else if (ExpectKeyword(token.text_, "properties"))
				{
					DeclarationProperties();
					return;
				}
				break;
			}
		}
	}
}

int Parser::FindCodeChunk(const std::string& name)
{
	for (uint32_t i = 0; i < shader_effect_.code_chunks_.size(); ++i)
	{
		CodeChunk* chunk = &shader_effect_.code_chunks_[i];
		if (name == chunk->name_) { return static_cast<int>(i); }
	}
	return -1;
}

//
// ShaderGenerator::ShaderGenerator(const AST& ast): ast(ast) {}
//
// void ShaderGenerator::GenerateShaders(const std::string& path)
// {
// 	const uint32_t pass_count = (uint32_t)ast.passes_.size();
// 	for (uint32_t i = 0; i < pass_count; i++)
// 	{
// 		const Pass& pass = ast.passes_[i];
// 		for (size_t s = 0; s < pass.shader_stages_.size(); ++s) { OutputShaderStage(path, pass.shader_stages_[s]); }
// 	}
// }
//
// void ShaderGenerator::OutputShaderStage(const std::string& path, const Pass::Stage& stage)
// {
// 	const CodeFragment* code_fragment = stage.code;
// 	if (code_fragment == nullptr)
// 		return;
//
// 	std::string fileName = path + code_fragment->name_.ToString() + "_" + ShaderType2Postfix(stage.stage) + ".glsl";
// 	std::ofstream file(fileName);
// 	std::string code = code_fragment->code_.ToString();
// 	file << "#version 330 core\n";
// 	file << "#define " << ShaderType2String(stage.stage) << "\n";
// 	file << code;
// 	file.close();
// }
//
// void GeneatePropertiesShaderCodeAndGetDefault(AST& ast, const DataBuffer& data_buffer, StringBuffer& out_defaults, StringBuffer& out_buffer)
// {
// 	// For each property, generate glsl code, output default value, (handle alignment)
// 	if (!ast.properties_.size())
// 	{
// 		uint32_t zeroSize = 0;
// 		out_defaults.AppendMemory(&zeroSize, sizeof(uint32_t));
// 		return;
// 	}
//
// 	// Add the local constants into the code.
// 	out_buffer.AppendFormat("\n\t\tlayout (std140, binding=7) uniform LocalConstants {\n\n");
//
// 	// For GPU the struct must be 16 bytes aligned. Track alignment
// 	uint32_t gpu_struct_alignment = 0;
//
// 	// In the defaults, write the type, size in '4 bytes' blocks, then data.
// 	ResourceType resource_type = ResourceType::kConstants;
// 	out_defaults.AppendMemory(&resource_type, sizeof(ResourceType));
//
// 	// Reserve space for later writing the correct value.
// 	char* buffer_size_memory = out_defaults.Allocate(sizeof(uint32_t));
//
// 	for (size_t i = 0; i < ast.properties_.size(); i++)
// 	{
// 		Property* property = ast.properties_[i];
//
// 		switch (property->type_)
// 		{
// 			case PropertyType::kFloat:
// 			{
// 				out_buffer.AppendFormat("\t\t\tfloat\t\t\t\t\t");
// 				out_buffer.AppendIndirectString(property->name_);
// 				out_buffer.AppendFormat(";\n");
//
// 				// Get default value and write it into default buffer
// 				if (property->data_index_ != INVALID_PROPERTY_DATA_INDEX)
// 				{
// 					float value = 0.0f;
// 					data_buffer.GetData(property->data_index_, value);
// 					out_defaults.AppendMemory(&value, sizeof(float));
// 				}
// 				// Update offset
// 				property->offset_in_bytes_ = gpu_struct_alignment * 4;
//
// 				++gpu_struct_alignment;
// 				break;
// 			}
//
// 			case PropertyType::kInt: { break; }
//
// 			case PropertyType::kRange: { break; }
//
// 			case PropertyType::kColor: { break; }
//
// 			case PropertyType::kVector: { break; }
// 		}
// 	}
//
// 	uint32_t tail_padding_size = 4 - (gpu_struct_alignment % 4);
// 	out_buffer.AppendFormat("\t\t\tfloat\t\t\t\t\tpad_tail[%u];\n\n", tail_padding_size);
// 	out_buffer.AppendFormat("\t\t} local_constants;\n\n");
//
// 	for (uint32_t v = 0; v < tail_padding_size; ++v)
// 	{
// 		float value = 0.0f;
// 		out_defaults.AppendMemory(&value, sizeof(float));
// 	}
//
// 	// Write the constant buffer size in bytes.
// 	uint32_t constants_buffer_size = (gpu_struct_alignment + tail_padding_size) * sizeof(float);
// 	memcpy(buffer_size_memory, &constants_buffer_size, sizeof(uint32_t));
// }

// void WriteEffectFile(AST& ast, const std::string& file_path)
// {
// 	BinarySerializer serializer(SerializerAction::kWrite, file_path);
// 	const uint32_t pass_count = (uint32_t)ast.passes_.size();
// 	for (uint32_t i = 0; i < pass_count; i++)
// 	{
// 		const Pass& pass = ast.passes_[i];
// 		for (size_t s = 0; s < pass.shader_stages_.size(); ++s)
// 		{
// 			const Pass::Stage& stage = pass.shader_stages_[s];
// 			serializer << stage.stage;
// 			serializer << stage.code->name_;
// 		}
// 	}
// }

// void DOWork(AST& ast, const std::string& binary_path, SerializerAction action)
// {
// 	{
// 		BinarySerializer serializer(action, binary_path);
// 		uint32_t pass_count = 0;
// 		if (serializer.GetAction() == SerializerAction::kWrite)
// 		{
// 			pass_count = (uint32_t)ast.passes_.size();
// 			serializer << pass_count;
// 		}
// 		else { serializer << pass_count; }
// 		ast.passes_.resize(pass_count);
// 		for (uint32_t i = 0; i < pass_count; i++)
// 		{
// 			Pass& pass = ast.passes_[i];
// 			uint32_t stage_count = 0;
// 			if (serializer.GetAction() == SerializerAction::kWrite)
// 			{
// 				stage_count = (uint32_t)pass.shader_stages_.size();
// 				serializer << stage_count;
// 			}
// 			else { serializer << stage_count; }
// 			pass.shader_stages_.resize(stage_count);
// 			for (size_t s = 0; s < stage_count; ++s)
// 			{
// 				Pass::Stage& stage = pass.shader_stages_[s];
// 				serializer << stage.stage;
// 				serializer << stage.code->name_;
// 			}
// 		}
// 	}
// }

void CompileHFX(const std::string& file_path)
{
	std::string content = FileReader(file_path).Read();
	DataBuffer data_buffer(256, 2048);
	Lexer lexer(content, data_buffer);
	Parser parser(lexer, data_buffer);
	parser.Parse();
	ShaderEffect& shader_effect = parser.GetShaderEffect();
	std::cout << shader_effect << std::endl;
	data_buffer.Print();

	{
		BinarySerializer serializer(SerializerAction::kWrite, "shader_effect.bin");
		serializer << shader_effect;
	}
	{
		ShaderEffect shader_effect2;
		BinarySerializer serializer(SerializerAction::kRead, "shader_effect.bin");
		serializer << shader_effect2;
	}

	// ShaderGenerator shader_generator(ast);
	// shader_generator.GenerateShaders(ST::PathManager::GetHFXDir());
	//
	// // Calculate Output File Path
	// std::string output_file_path = ""; //TODO
	//
	// StringBuffer out_defaults;
	// StringBuffer out_buffer;
	// GeneatePropertiesShaderCodeAndGetDefault(ast, data_buffer, out_defaults, out_buffer);
	//
	// DOWork(ast, ST::PathManager::GetHFXDir() + "shader_effect.bin", SerializerAction::kWrite);
	// AST ast2;
	// DOWork(ast2, ST::PathManager::GetHFXDir() + "shader_effect.bin", SerializerAction::kRead);
}
}
