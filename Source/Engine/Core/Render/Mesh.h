#pragma once
#include "Core.h"
#include "vec2.hpp"
#include "vec3.hpp"
#include "VertexArray.h"

namespace ST {
class Material;

class Camera;

struct Vertex {
	Vertex() = default;

	Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& texCoord):
		_pos(pos), _normal(normal), _texCoord(texCoord) {}

	glm::vec3 _pos;

	glm::vec3 _normal;

	glm::vec2 _texCoord;
};

class Mesh {
public:
	Mesh(ST_VECTOR<Vertex>&& verts, ST_VECTOR<unsigned int>&& indices,
		const ST_VECTOR<ST_REF<Material>>& materials):
		_verts(std::forward<ST_VECTOR<Vertex>>(verts)), _indices(std::forward<ST_VECTOR<unsigned int>>(indices)),
		_materials(materials), _vertexArray(ST_MAKE_REF<VertexArray>()) {
		SetUpMesh();
		if (indices.size() != 0)
			_hasIndices = true;
	}

	void SetUpMesh();

	ST_VECTOR<Vertex> _verts;

	ST_VECTOR<unsigned int> _indices;

	ST_VECTOR<ST_REF<Material>> _materials;

	bool _hasIndices = false;

	ST_REF<VertexArray> _vertexArray;

};
}
