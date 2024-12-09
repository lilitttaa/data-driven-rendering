#pragma once

#include "Buffer.h"
#include "Core.h"

namespace ST {
class VertexBuffer;

class IndexBuffer;

class VertexArray {

public:
	VertexArray();

	~VertexArray() {
		glDeleteVertexArrays(1, &_arraryId);
	}

	inline void Bind() const {
		glBindVertexArray(_arraryId);
	}

	inline void UnBind() const {
		glBindVertexArray(0);
	}

	void AddVertexBuffer(ST_REF<VertexBuffer> vertexBuffer);

	void SetIndexBuffer(ST_REF<IndexBuffer> indexBuffer);

	ST_VECTOR<ST_REF<VertexBuffer>> _vertexBuffers;

	ST_REF<IndexBuffer> _indexBuffer;

private:
	unsigned int _arraryId;

};
}
