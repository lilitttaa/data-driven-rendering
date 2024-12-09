#pragma once
#include"Core.h"
#include"ShaderDataType.h"
#include "pch.h"
#include "Shader.h"
#include "Texture2D.h"
#include "VertexArray.h"

namespace ST {
class Texture2D;

class RenderBuffer;

struct LayoutParam;

struct BufferLayout {
	ST_VECTOR<LayoutParam> s_Params;

	inline BufferLayout() {}

	inline BufferLayout(std::initializer_list<LayoutParam> initialList): s_Params(initialList) {}

	inline ST_VECTOR<LayoutParam>::iterator begin() {
		return s_Params.begin();
	}

	inline ST_VECTOR<LayoutParam>::iterator end() {
		return s_Params.end();
	}

	inline ST_VECTOR<LayoutParam>::const_iterator begin() const {
		return s_Params.begin();
	}

	inline ST_VECTOR<LayoutParam>::const_iterator end() const {
		return s_Params.end();
	}
};

struct LayoutParam {
	LayoutParam(ShaderDataType type, ST_STRING name, bool normalized = false):
		_name(name),
		normalized(normalized),
		_type(type) {}

	ST_STRING _name;

	bool normalized;

	ShaderDataType _type;
};

enum class BufferMode {
	STATIC_BUFFER=0,
	DYNAMIC_BUFFER
};

class VertexBuffer {
private:
	unsigned int _bufferId;

	BufferLayout _bufferLayout;

	BufferMode _mode;

public:
	friend class VertexArray;

	VertexBuffer(const float* verts, uint32_t size, BufferMode mode);

	~VertexBuffer() {
		glDeleteBuffers(1, &_bufferId);
	}

	// static Ref<VertexBuffer> CreateVertexBuffer(const float* Verts);

	inline void Bind() {
		glBindBuffer(GL_ARRAY_BUFFER, _bufferId);
	}

	inline void UnBind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SetLayout(const BufferLayout& bufferLayer) {
		_bufferLayout = bufferLayer;
	}

	inline BufferLayout GetBufferLayout() const {
		return _bufferLayout;
	}
};

class IndexBuffer {
private:
	unsigned int _bufferId;

public:
	friend class VertexArray;

	IndexBuffer(const uint32_t* indexs, uint32_t size);

	~IndexBuffer() {
		glDeleteBuffers(1, &_bufferId);
	}

	inline void Bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferId);
	}

	inline void UnBind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};

class FrameBuffer {
private:
	class RenderBuffer {
	private:
		friend FrameBuffer;

		unsigned int _bufferId;

	public:
		RenderBuffer(unsigned int width, unsigned int height);

		inline void Bind() {
			glBindRenderbuffer(GL_RENDERBUFFER, _bufferId);
		}
		
		inline void UnBind() {
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	};

	unsigned int _bufferId;

	ST_REF<Texture2D> _postProcessingTexture;

	ST_REF<RenderBuffer> _renderBuffer;

public:
	FrameBuffer(unsigned int width, unsigned int height);

	~FrameBuffer() {
		glDeleteBuffers(1, &_bufferId);
	}

	inline void Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, _bufferId);
		_renderBuffer->Bind();
	}

	inline void BindTexture() {
		_postProcessingTexture->Bind(0);
	}

	inline void UnBind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		_renderBuffer->UnBind();
	}
};

}
