#include "Model.h"

#include "Material.h"
#include "Mesh.h"
#include "PathManager.h"
#include "Texture2D.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

ST::Model::Model(const ST_STRING& path) {
	LoadModel(path);
}



void ST::Model::LoadModel(const ST_STRING& path) {
	_dicPath = path.substr(0, path.find_last_of("/") + 1);
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(PathManager::GetFullPath(path), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		ST_ERROR("Error assimp: %s", import.GetErrorString());
	}
	ProcessNode(scene->mRootNode, scene);
}

void ST::Model::ProcessNode(const aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		_meshes.push_back(ST_MAKE_REF<Mesh>(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene)));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		ProcessNode(node->mChildren[i], scene);
	}
}

ST::Mesh ST::Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene) {
	ST_VECTOR<Vertex> verts;
	ST_VECTOR<unsigned int> indices;
	ST_VECTOR<Texture2D> textures;
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;;
		vertex._pos.x = mesh->mVertices[i].x;
		vertex._pos.y = mesh->mVertices[i].y;
		vertex._pos.z = mesh->mVertices[i].z;

		if (mesh->HasNormals()) {
			vertex._normal.x = mesh->mNormals[i].x;
			vertex._normal.y = mesh->mNormals[i].y;
			vertex._normal.z = mesh->mNormals[i].z;
		}

		if (mesh->mTextureCoords[0]) {
			vertex._texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex._texCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex._texCoord = {0, 0};
		}
		verts.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* aiMaterials = scene->mMaterials[mesh->mMaterialIndex];

	ST_VECTOR<ST_REF<Material>> materials;
	GetTextures(materials, aiMaterials, aiTextureType_AMBIENT);
	GetTextures(materials, aiMaterials, aiTextureType_DIFFUSE);
	GetTextures(materials, aiMaterials, aiTextureType_SPECULAR);

	return Mesh(std::move(verts),std::move(indices), materials);
}

void ST::Model::GetTextures(ST_VECTOR<ST_REF<Material>>& materials, aiMaterial* aiMaterials, aiTextureType type) {
	auto count = aiMaterials->GetTextureCount(type);
	for (unsigned int i = 0; i < count; ++i) {
		if (materials.size() <= i) {
			materials.emplace_back(ST_MAKE_REF<Material>());
		}
		aiString str;
		aiMaterials->GetTexture(type, i, &str);
		switch (type) {
			case aiTextureType_AMBIENT: materials[i]->_ambientTexPath = _dicPath + str.C_Str();
				break;
			case aiTextureType_DIFFUSE: materials[i]->_diffuseTexPath = _dicPath + str.C_Str();
				if (materials[i]->_ambientTexPath == MATERIAL_DEFAULT_TEXTURE_PATH)
					materials[i]->_ambientTexPath = materials[i]->_diffuseTexPath;
				break;
			case aiTextureType_SPECULAR: materials[i]->_specularTexPath = _dicPath + str.C_Str();
				break;
		}
	}
}
