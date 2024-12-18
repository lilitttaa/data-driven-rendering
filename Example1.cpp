#include <direct.h>
#include <iostream>
#include "Application.h"
#include "PathManager.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "gtc/type_ptr.hpp"
#include "HFX/HFX.h"

using namespace ST;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
		HFX::CompileHFX(PathManager::GetHFXDir() + "Ball.hfx");
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
		LoadFileToStr(PathManager::GetHFXDir() + "Ball_vt.glsl", vertexShaderSourceStr);
		const char* vertexShaderSource = vertexShaderSourceStr.c_str();

		std::string fragmentShaderSourceStr;
		LoadFileToStr(PathManager::GetHFXDir() + "Ball_fg.glsl", fragmentShaderSourceStr);
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
		// std::cout << "Ball Position: " << ball.posX << ", " << ball.posY << std::endl;
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

	virtual void Init() override {}

	virtual void Tick(float deltaTime) override {}

	virtual void Render(float deltaTime) override {}

	virtual void Destroy() override {}
};

ST::Application* CreateApplication() { return new Example1(); }
