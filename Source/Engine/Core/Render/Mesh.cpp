#include "Mesh.h"

#include "Camera.h"
#include "Material.h"

void ST::Mesh::SetUpMesh() {
	auto vertexBuffer = ST_MAKE_REF<VertexBuffer>((float*)_verts.data(), sizeof(Vertex) * _verts.size(),
		BufferMode::STATIC_BUFFER);
	vertexBuffer->SetLayout({
		{Float3, "v_Pos"},
		{Float3, "v_Normal"},
		{Float2, "v_TexCoord"}
	});
	_vertexArray->AddVertexBuffer(vertexBuffer);
	auto idxBuffer = ST_MAKE_REF<IndexBuffer>(_indices.data(), sizeof(unsigned int) * _indices.size());
	_vertexArray->SetIndexBuffer(idxBuffer);
}

