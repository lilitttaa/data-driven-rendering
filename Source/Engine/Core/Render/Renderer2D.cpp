#include "Renderer2D.h"

#include <GL/gl.h>
#include "AppWindow.h"
#include "Buffer.h"
#include "Font.h"
#include "FontCharacter.h"
#include "PathManager.h"
#include "ResourceManager.h"
#include "VertexArray.h"
#include "ext/matrix_clip_space.hpp"
#include "ext/matrix_transform.hpp"
#include "UI/Brush.h"

ST::Renderer2D::Renderer2D(AppWindow* appWindow):
	_appWindow(appWindow),
	_vertexArray(ST_MAKE_REF<VertexArray>()),
	_textVertexArray(ST_MAKE_REF<VertexArray>()),
	_shader(ST_MAKE_REF<Shader>("/Resource/OpenGLShader/UI_Shader.vt.glsl",
		"/Resource/OpenGLShader/UI_Shader.fg.glsl")),
	_texShader(ST_MAKE_REF<Shader>("/Resource/OpenGLShader/Text_Shader.vt.glsl",
		"/Resource/OpenGLShader/Text_Shader.fg.glsl")),
	_texture(ST_MAKE_REF<Texture2D>("/Resource/NoManSky.jpg")),
	_font(ST_MAKE_REF<Font>()) {
#pragma region /** Image vertex array */
	float verts[] = {
		// positions
		1, 1, 1, 1,   // 右上角
		1, -1, 1, 0,  // 右下角
		-1, -1, 0, 0, // 左下角
		-1, 1, 0, 1   // 左上角
	};
	uint32_t index[] = {
		0, 1, 3,
		1, 2, 3
	};
	auto vertexBuffer = ST_MAKE_REF<VertexBuffer>(verts, sizeof(verts), BufferMode::STATIC_BUFFER);
	vertexBuffer->SetLayout({
		{Float2, "v_Pos"},
		{Float2, "v_TexCoord"}
	});
	auto indexBuffer = ST_MAKE_REF<IndexBuffer>(index, sizeof(index));
	_vertexArray->AddVertexBuffer(vertexBuffer);
	_vertexArray->SetIndexBuffer(indexBuffer);
#pragma endregion

#pragma region /** Text vertext array */
	float textVerts[] = {
		// positions
		1, 1, 1, 0,   // 右上角
		1, -1, 1, 1,  // 右下角
		-1, -1, 0, 1, // 左下角
		-1, 1, 0, 0   // 左上角
	};
	uint32_t textIndex[] = {
		0, 1, 3,
		1, 2, 3
	};
	vertexBuffer = ST_MAKE_REF<VertexBuffer>(textVerts, sizeof(textVerts), BufferMode::DYNAMIC_BUFFER);
	vertexBuffer->SetLayout({
		{Float2, "v_Pos"},
		{Float2, "v_TexCoord"}
	});
	auto textIndexBuffer = ST_MAKE_REF<IndexBuffer>(textIndex, sizeof(textIndex));
	_textVertexArray->AddVertexBuffer(vertexBuffer);
	_textVertexArray->SetIndexBuffer(textIndexBuffer);
#pragma endregion

	_font->Init(0, 48);
}

void ST::Renderer2D::DrawQuad(const Rect& rect, const Brush& brush) {
	_vertexArray->Bind();
	_vertexArray->_vertexBuffers[0]->Bind();
	glm::mat4 transformMat = CreateTransformMat(rect);
	_shader->UseShader();
	_shader->SetVec4("f_Color", brush._color);
	_shader->SetMat4("v_TransformMat", transformMat);
	auto texture=ResourceManager::GetResourceManager().LoadTexture(brush._texPath);
	if (texture == nullptr) {
		_texture->Bind(0);
		_shader->SetInt("f_Texture", 0);
	}
	else {
		texture->Bind(1);
		_shader->SetInt("f_Texture", 1);
	}
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ST::Renderer2D::DrawPoint(glm::vec2&& pos, float size, glm::vec3 color) {}

void ST::Renderer2D::DrawSingleLineText(glm::vec2 pos, glm::vec3 color, float scale, const ST_STRING& text) {
	_textVertexArray->Bind();
	_textVertexArray->_vertexBuffers[0]->Bind();

	_texShader->UseShader();
	_texShader->SetVec3("f_Color", color);

	for (const auto& ch : text) {
		auto fontCharacter = _font->GetFontCharacter(ch);
		fontCharacter->_texture->Bind(3);
		_texShader->SetInt("f_Texture", 3);
		glm::mat4 transformMat = CreateTransformMat(Rect({
				pos.x + static_cast<float>(fontCharacter->_bearing.x) * scale,
				pos.y + static_cast<float>(fontCharacter->_bearing.y - fontCharacter->_size.y) * scale
			},
			glm::vec2(static_cast<float>(fontCharacter->_size.x) * scale,
				static_cast<float>(fontCharacter->_size.y) * scale)));
		_texShader->SetMat4("v_TransformMat", transformMat);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		pos.x += static_cast<float>(fontCharacter->_advance >> 6) * scale;
	}
	_textVertexArray->UnBind();
	/*auto fontCharacter = _font->GetFontCharacter(c);
	fontCharacter->_texture->Bind(3);
	fontCharacter->_size;
	glm::mat4 transformMat = CreateTransformMat(Rect({rect._x, rect._y}, fontCharacter->_size));
	_texShader->SetMat4("v_TransformMat", transformMat);

	_texShader->SetInt("f_Texture", 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	_textVertexArray->UnBind();*/
}

void ST::Renderer2D::DrawSingleChar(Rect&& rect, ST_FONT_CHAR c) {
	_textVertexArray->Bind();
	_textVertexArray->_vertexBuffers[0]->Bind();

	_texShader->UseShader();
	_texShader->SetVec3("f_Color", glm::vec3(0.1, 0.1, 0.1));

	auto fontCharacter = _font->GetFontCharacter(c);
	fontCharacter->_texture->Bind(3);
	fontCharacter->_size;
	glm::mat4 transformMat = CreateTransformMat(Rect({rect._pos.x, rect._pos.y}, fontCharacter->_size));
	_texShader->SetMat4("v_TransformMat", transformMat);

	_shader->SetInt("f_Texture", 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

glm::mat4 ST::Renderer2D::CreateTransformMat(const Rect& rect) {
	double screenXSize, screenYSize;
	if (_appWindow) {
		_appWindow->GetWindowSize(screenXSize, screenYSize);
	}
	return glm::mat4(
		rect._size.x / screenXSize, 0, 0, 0,
		0, rect._size.y / screenYSize, 0, 0,
		0, 0, 1, 0,
		-1 + 2 * (rect._size.x / 2 + rect._pos.x) / screenXSize,
		-1 + 2 * (rect._size.y / 2 + rect._pos.y) / screenYSize, 0, 1
	);
}
