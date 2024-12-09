#include "Material.h"

#include "ResourceManager.h"
#include "Texture2D.h"

void ST::Material::Bind() {
	ST_REF<Texture2D> _ambientTex  = ResourceManager::GetResourceManager().LoadTexture(_ambientTexPath);
	ST_REF<Texture2D> _diffuseTex  = ResourceManager::GetResourceManager().LoadTexture(_diffuseTexPath);
	ST_REF<Texture2D> _specularTex = ResourceManager::GetResourceManager().LoadTexture(_specularTexPath);
	_ambientTex->Bind(_idx * 3);
	_diffuseTex->Bind(_idx * 3 + 1);
	_specularTex->Bind(_idx * 3 + 2);
}

void ST::Material::UnBind() {
	ST_REF<Texture2D> _ambientTex  = ResourceManager::GetResourceManager().LoadTexture(_ambientTexPath);
	ST_REF<Texture2D> _diffuseTex  = ResourceManager::GetResourceManager().LoadTexture(_diffuseTexPath);
	ST_REF<Texture2D> _specularTex = ResourceManager::GetResourceManager().LoadTexture(_specularTexPath);
	_ambientTex->UnBind(_idx * 3);
	_diffuseTex->UnBind(_idx * 3 + 1);
	_specularTex->UnBind(_idx * 3 + 2);
}
