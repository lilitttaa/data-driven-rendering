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

ST::Application* CreateApplication() { return new Example1(); }

typedef std::string StringRef; //TODO
template <typename T>
typedef std::vector<T> VectorRef; //TODO

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
	const char* text;
	uint32_t length;
	typedef TokenType Type;
	uint32_t line;
};

enum class ShaderStage
{
	Vertex, Fragment, Geometry, Compute, Hull, Domain, Count
};

bool expectKeyword(const StringRef& text, const StringRef& expected_keyword)
{
	if (text.length() != expected_keyword.length())
		return false;

	for (uint32_t i = 0; i < expected_keyword.length(); ++i)
	{
		if (text[i] != expected_keyword[i])
			return false;
	}
	return true;
}

class Lexer
{
public:
	Lexer(const StringRef& source): position(source.c_str()), line(1), column(0), hasError(false), errorLine(1) {}

	bool IsEndOfLine(char c) { return (c == '\n' || c == '\r'); }
	bool IsWhitespace(char c) { return (c == ' ' || c == '\t' || IsEndOfLine(c)); }
	bool IsAlpha(char c) { return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')); }
	bool IsNumber(char c) { return (c >= '0' && c <= '9'); }

	void skip_whitespace()
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

	void parse_number(Lexer* lexer)
	{
		char c = lexer->position[0];

		// Parse the following literals:
		// 58, -58, 0.003, 4e2, 123.456e-67, 0.1E4f
		// 1. Sign detection
		int32_t sign = 1;
		if (c == '-')
		{
			sign = -1;
			++lexer->position;
		}

		// 2. Heading zeros (00.003)
		if (*lexer->position == '0')
		{
			++lexer->position;

			while (*lexer->position == '0')
				++lexer->position;
		}
		// 3. Decimal part (until the point)
		int32_t decimal_part = 0;
		if (*lexer->position > '0' && *lexer->position <= '9')
		{
			decimal_part = (*lexer->position - '0');
			++lexer->position;

			while (*lexer->position != '.' && IsNumber(*lexer->position))
			{
				decimal_part = (decimal_part * 10) + (*lexer->position - '0');

				++lexer->position;
			}
		}
		// 4. Fractional part
		int32_t fractional_part = 0;
		int32_t fractional_divisor = 1;

		if (*lexer->position == '.')
		{
			++lexer->position;

			while (IsNumber(*lexer->position))
			{
				fractional_part = (fractional_part * 10) + (*lexer->position - '0');
				fractional_divisor *= 10;

				++lexer->position;
			}
		}

		// 5. Exponent (if present)
		if (*lexer->position == 'e' || *lexer->position == 'E') { ++lexer->position; }

		double parsed_number = (double)sign * (decimal_part + ((double)fractional_part / fractional_divisor));
		add_data(lexer->data_buffer, parsed_number); //TODO
	}

	void next_token(Token& token)
	{
		// Skip all whitespace first so that the token is without them.
		skip_whitespace();

		// Initialize token
		token.type = TokenType::Token_Unknown;
		token.text = position;
		token.length = 1;
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

				token.text = position;

				while (position[0] &&
				       position[0] != '"')
				{
					if ((position[0] == '\\') &&
					    position[1]) { ++position; }
					++position;
				}

				token.length = static_cast<uint32_t>(position - token.text);
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

					token.length = static_cast<uint32_t>(position - token.text);
				} // Numbers: handle also negative ones!
				else if (IsNumber(c) || c == '-')
				{
					// Backtrack to start properly parsing the number
					--position;
					parse_number(lexer);
					// Update token and calculate correct length.
					token.type = TokenType::Token_Number;
					token.length = static_cast<uint32_t>(position - token.text);
				}
				else { token.type = TokenType::Token_Unknown; }
			}
			break;
		}
	}

	bool equalToken(Token& token, TokenType expected_type)
	{
		next_token(token);
		return token.type == expected_type;
	}

	// The same to equalToken but with error handling.
	bool expectToken(Token& token, TokenType expected_type)
	{
		if (hasError)
			return true;

		next_token(token);
		hasError = token.type != expected_type;
		if (hasError)
		{
			// Save line of error
			errorLine = line;
		}
		return !hasError;
	}

protected:
	char const* position;
	uint32_t line;
	uint32_t column;
	bool hasError;
	uint32_t errorLine;
	StringRef dataBuffer; //TODO
};

struct CodeFragment
{
	StringRef name;
};

struct Shader
{
	VectorRef<CodeFragment> codeFragments;
};

class Parser
{
public:
	Parser(Lexer& lexer): lexer(lexer) {}

protected:
	Lexer& lexer;
	Shader shader;

	void generateAST(Parser* parser)
	{
		// Read source text until the end.
		// The main body can be a list of declarations.
		bool parsing = true;

		while (parsing)
		{
			Token token;
			// nextToken(parser->lexer, token);
			parser->lexer.next_token(token);

			switch (token.type)
			{
				case TokenType::Token_Identifier:
				{
					identifier(parser, token);
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

	inline void declarationShader(Parser* parser)
	{
		// Parse name
		Token token;
		if (!parser->lexer.expectToken(token, TokenType::Token_Identifier)) { return; }

		// Cache name string
		StringRef name = token.text;

		if (!parser->lexer.expectToken(token, TokenType::Token_OpenBrace)) { return; }

		while (!parser->lexer.equalToken(token, TokenType::Token_CloseBrace)) { identifier(parser, token); }
	}

	void directive_identifier(Parser* parser, const Token& token, CodeFragment& code_fragment)
	{
		Token new_token;
		for (uint32_t i = 0; i < token.length; ++i)
		{
			char c = *(token.text + i);

			switch (c)
			{
				case 'i':
				{
					// Search for the pattern 'if defined'
					if (expectKeyword(token.text, "if"))
					{
						next_token(parser->lexer, new_token);

						if (expectKeyword(new_token.text, "defined"))
						{
							next_token(parser->lexer, new_token);

							// Use 0 as not set value for the ifdef depth.
							++code_fragment.ifdef_depth;

							if (expectKeyword(new_token.text, "VERTEX"))
							{
								code_fragment.stage_ifdef_depth[Stage::Vertex] = code_fragment.ifdef_depth;
								code_fragment.current_stage = Stage::Vertex;
							}
							else if (expectKeyword(new_token.text, "FRAGMENT"))
							{
								code_fragment.stage_ifdef_depth[Stage::Fragment] = code_fragment.ifdef_depth;
								code_fragment.current_stage = Stage::Fragment;
							}
							else if (expectKeyword(new_token.text, "COMPUTE"))
							{
								code_fragment.stage_ifdef_depth[Stage::Compute] = code_fragment.ifdef_depth;
								code_fragment.current_stage = Stage::Compute;
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
						parser->lexer.next_token(new_token);

						if (expectKeyword(new_token.text, "include"))
						{
							parser->lexer.next_token(new_token);

							code_fragment.includes.emplace_back(new_token.text);
							code_fragment.includes_flags.emplace_back((uint32_t)code_fragment.current_stage);
						}
						else if (expectKeyword(new_token.text, "include_hfx"))
						{
							parser->lexer.next_token(new_token);

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
						if (code_fragment.stage_ifdef_depth[Stage::Vertex] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[Stage::Vertex] = 0xffffffff;
							code_fragment.current_stage = Stage::Count;
						}
						else if (code_fragment.stage_ifdef_depth[Stage::Fragment] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[Stage::Fragment] = 0xffffffff;
							code_fragment.current_stage = Stage::Count;
						}
						else if (code_fragment.stage_ifdef_depth[Stage::Compute] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[Stage::Compute] = 0xffffffff;
							code_fragment.current_stage = Stage::Count;
						}

						--code_fragment.ifdef_depth;

						return;
					}
					break;
				}
			}
		}
	}

	void uniform_identifier(Parser* parser, const Token& token, CodeFragment& code_fragment)
	{
		for (uint32_t i = 0; i < token.length; ++i)
		{
			char c = *(token.text + i);

			switch (c)
			{
				case 'i':
				{
					if (expectKeyword(token.text, "image2D"))
					{
						// Advance to next token to get the name
						Token name_token;
						parser->lexer.next_token(name_token);

						CodeFragment::Resource resource = {hydra::graphics::ResourceType::TextureRW, name_token.text};
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
						parser->lexer.next_token(name_token);

						CodeFragment::Resource resource = {hydra::graphics::ResourceType::Texture, name_token.text};
						code_fragment.resources.emplace_back(resource);
					}
					break;
				}
			}
		}
	}

	inline void declarationGlsl(Parser* parser)
	{
		// Parse name
		Token token;
		if (!parser->lexer.expectToken(token, TokenType::Token_Identifier)) { return; }
		CodeFragment code_fragment = {};
		// Cache name string
		code_fragment.name = token.text;

		for (size_t i = 0; i < Stage::Count; i++) { code_fragment.stage_ifdef_depth[i] = 0xffffffff; }

		if (!parser->lexer.expectToken(token, TokenType::Token_OpenBrace)) { return; }

		// Advance token and cache the starting point of the code.
		parser->lexer.next_token(token);
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
				parser->lexer.next_token(token);

				directive_identifier(parser, token, code_fragment);
			}
			else if (token.type == TokenType::Token_Identifier)
			{
				// Parse uniforms to add resource dependencies if not explicit in the HFX file.
				if (expectKeyword(token.text, "uniform"))
				{
					parser->lexer.next_token(token);

					uniform_identifier(parser, token, code_fragment);
				}
			}

			// Only advance token when we are inside the glsl braces, otherwise will skip the following glsl part.
			if (open_braces)
				parser->lexer.next_token(token);
		}

		// Calculate code string length using the token before the last close brace.
		code_fragment.code.length = token.text.text - code_fragment.code.text;

		parser->shader.code_fragments.emplace_back(code_fragment);
	}

	inline void directiveIdentifier(Parser* parser, const Token& token, CodeFragment& code_fragment)
	{
		Token new_token;
		for (uint32_t i = 0; i < token.length; ++i)
		{
			char c = *(token.text + i);

			switch (c)
			{
				case 'i':
				{
					// Search for the pattern 'if defined'
					if (expectKeyword(token.text, "if"))
					{
						parser->lexer.next_token(new_token);

						if (expectKeyword(new_token.text, "defined"))
						{
							parser->lexer.next_token(new_token);

							// Use 0 as not set value for the ifdef depth.
							++code_fragment.ifdef_depth;

							if (expectKeyword(new_token.text, "VERTEX"))
							{
								code_fragment.stage_ifdef_depth[ShaderStage::Vertex] = code_fragment.ifdef_depth;
								code_fragment.current_stage = ShaderStage::Vertex;
							}
							else if (expectKeyword(new_token.text, "FRAGMENT"))
							{
								code_fragment.stage_ifdef_depth[ShaderStage::Fragment] = code_fragment.ifdef_depth;
								code_fragment.current_stage = ShaderStage::Fragment;
							}
							else if (expectKeyword(new_token.text, "COMPUTE"))
							{
								code_fragment.stage_ifdef_depth[ShaderStage::Compute] = code_fragment.ifdef_depth;
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
						parser->lexer.next_token(new_token);

						if (expectKeyword(new_token.text, "include"))
						{
							parser->lexer.next_token(new_token);

							code_fragment.includes.emplace_back(new_token.text);
							code_fragment.includes_stage.emplace_back(code_fragment.current_stage);
						}

						return;
					}
					break;
				}
				case 'e':
				{
					if (expectKeyword(token.text, "endif"))
					{
						if (code_fragment.stage_ifdef_depth[ShaderStage::Vertex] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[ShaderStage::Vertex] = 0xffffffff;
							code_fragment.current_stage = ShaderStage::Common;
						}
						else if (code_fragment.stage_ifdef_depth[ShaderStage::Fragment] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[ShaderStage::Fragment] = 0xffffffff;
							code_fragment.current_stage = ShaderStage::Common;
						}
						else if (code_fragment.stage_ifdef_depth[ShaderStage::Compute] == code_fragment.ifdef_depth)
						{
							code_fragment.stage_ifdef_depth[ShaderStage::Compute] = 0xffffffff;
							code_fragment.current_stage = ShaderStage::Common;
						}

						--code_fragment.ifdef_depth;

						return;
					}
					break;
				}
			}
		}
	}

	inline void declarationShaderStage(Parser* parser, const CodeFragment** out_fragment)
	{
		Token token;
		if (!parser->lexer.expectToken(token, TokenType::Token_Equals)) { return; }

		if (!parser->lexer.expectToken(token, TokenType::Token_Identifier)) { return; }

		*out_fragment = findCodeFragment(parser, token.text);
	}

	inline void passIdentifier(Parser* parser, const Token& token, Pass& pass)
	{
		// Scan the name to know which stage we are parsing    
		for (uint32_t i = 0; i < token.length; ++i)
		{
			char c = *(token.text + i);

			switch (c)
			{
				case 'c':
				{
					if (expectKeyword(token.text, "compute"))
					{
						declarationShaderStage(parser, &pass.cs);
						return;
					}
					break;
				}

				case 'v':
				{
					if (expectKeyword(token.text, "vertex"))
					{
						declarationShaderStage(parser, &pass.vs);
						return;
					}
					break;
				}

				case 'f':
				{
					if (expectKeyword(token.text, "fragment"))
					{
						declarationShaderStage(parser, &pass.fs);
						return;
					}
					break;
				}
			}
		}
	}

	inline void declarationPass(Parser* parser)
	{
		Token token;
		if (!parser->lexer.expectToken(token, TokenType::Token_Identifier)) { return; }

		Pass pass = {};
		// Cache name string
		pass.name = token.text;

		if (!parser->lexer.expectToken(token, TokenType::Token_OpenBrace)) { return; }
		while (!parser->lexer.equalToken(token, TokenType::Token_CloseBrace)) { passIdentifier(parser, token, pass); }
	}

	inline void identifier(Parser* parser, const Token& token)
	{
		// Scan the name to know which 
		for (uint32_t i = 0; i < token.length; ++i)
		{
			switch (token.text[i])
			{
				case 's':
				{
					if (expectKeyword(token.text, "shader"))
					{
						declarationShader(parser);
						return;
					}

					break;
				}

				case 'g':
				{
					if (expectKeyword(token.text, "glsl"))
					{
						declarationGlsl(parser);
						return;
					}
					break;
				}

				case 'p':
				{
					if (expectKeyword(token.text, "pass"))
					{
						declarationPass(parser);
						return;
					}
					break;
				}
			}
		}
	}

	const CodeFragment* findCodeFragment(const Parser* parser, const StringRef& name)
	{
		for (uint32_t i = 0; i < parser->shader.codeFragments.size(); ++i)
		{
			const CodeFragment* type = &parser->shader.codeFragments[i];
			if (name == type->name) { return type; }
		}
		return nullptr;
	}
};

class CodeGenerator
{};
}

void compileHFX(const StringRef& fullPath, const StringRef& outputDir, const StringRef& outputName)
{
	std::string simpleFullscreenSourceStr;
	LoadFileToStr(GetHFXDir() + "SimpleFullscreen.hfx", simpleFullscreenSourceStr);

	HFX::Lexer lexer(simpleFullscreenSourceStr);
	HFX::Parser parser(lexer);
	parser.generateAST();

	HFX::CodeGenerator code_generator(parser);
	code_generator.generateShaderPermutations(outputDir);

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
