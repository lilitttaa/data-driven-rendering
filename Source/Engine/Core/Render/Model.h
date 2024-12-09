#pragma once
#include "Core.h"
#include "assimp/material.h"

struct aiMesh;

struct aiScene;

struct aiNode;

namespace ST {
class Material;

class Camera;

class Shader;

class Mesh;

class Model {
public:
	Model(const ST_STRING& path);

	Model(const ST_VECTOR<ST_REF<Mesh>>& meshes): _meshes(meshes) {}

	ST_VECTOR<ST_REF<Mesh>> _meshes;

private:
	void LoadModel(const ST_STRING& path);

	void ProcessNode(const aiNode* node, const aiScene* scene);

	Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);

	void GetTextures(ST_VECTOR<ST_REF<Material>>& materials, aiMaterial* aiMaterials, aiTextureType type);

	ST_STRING _dicPath;
};
}
