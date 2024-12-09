#include "Buffer.h"

#include "Texture2D.h"

namespace ST {
VertexBuffer::VertexBuffer(const float* verts, uint32_t size, BufferMode mode): _mode(mode) {
	glGenBuffers(1, &_bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, _bufferId);
	if (mode == BufferMode::STATIC_BUFFER)
		glBufferData(GL_ARRAY_BUFFER, size, verts,GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, size, verts,GL_DYNAMIC_DRAW);
}

// Ref<VertexBuffer> VertexBuffer::CreateVertexBuffer(const float* verts)
// {
//     return MakeRef<VertexBuffer>(verts);
// }

IndexBuffer::IndexBuffer(const uint32_t* Indexs, uint32_t size) {
	glGenBuffers(1, &_bufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, Indexs,GL_STATIC_DRAW);
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height) {
	glGenFramebuffers(1, &_bufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _bufferId);

	_postProcessingTexture = ST_MAKE_REF<Texture2D>(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _postProcessingTexture->_textureId, 0);
	
	_renderBuffer          = ST_MAKE_REF<RenderBuffer>(width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderBuffer->_bufferId);
	// now actually attach it
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ST_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::RenderBuffer::RenderBuffer(unsigned int width, unsigned int height) {
	glGenRenderbuffers(1, &_bufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, _bufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

}
}
