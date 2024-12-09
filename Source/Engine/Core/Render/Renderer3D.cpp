#include "Renderer3D.h"

#include "Camera.h"
#include "CameraController.h"
#include "CubeMap.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Material.h"
#include "ResourceManager.h"
#include "VertexArray.h"
#include "gtc/quaternion.hpp"
#include "gtx/transform.hpp"
#include "Math/MathLibrary.h"
#include "Math/Transform.h"
#include "UI/ImguiPanel.h"

namespace ST {
struct Vertex;
}

ST::Renderer3D::Renderer3D(AppWindow* window):
	_window(window){
	auto cubeMapShader=ResourceManager::GetResourceManager().LoadShader(
		"/Resource/OpenGLShader/SkyBox.vt.glsl",
		"/Resource/OpenGLShader/SkyBox.fg.glsl");
	cubeMapShader->UseShader();
	_skyBox=ST_MAKE_REF<CubeMap>(ST_VECTOR<ST_STRING>{
		"/Resource/skybox/right.jpg",
		"/Resource/skybox/left.jpg",
		"/Resource/skybox/top.jpg",
		"/Resource/skybox/bottom.jpg",
		"/Resource/skybox/back.jpg",
		"/Resource/skybox/front.jpg"
	});
	
	auto shader=ResourceManager::GetResourceManager().LoadShader(
		"/Resource/OpenGLShader/PureColorShader.vt.glsl",
		"/Resource/OpenGLShader/PureColorShader.fg.glsl");
	shader->UseShader();
		_frameBuffer=ST_MAKE_REF<FrameBuffer>(window->_width, _window->_height);
	}

void ST::Renderer3D::SetLight() {
	_shader->SetDirLight("f_DirLight", _dirLight);
	_shader->SetPointLight("f_PointLight", _pointLight);
	ImguiPanel::CreateDirLightPanel("Dir Light", _dirLight);
	ImguiPanel::CreatePointLightPanel("Point Light", _pointLight);
}

void ST::Renderer3D::BeginDraw(ST_REF<Shader> shader, ST_REF<Camera> camera) {
	_shader = shader;
	_shader->UseShader();
	_shader->SetMat4("v_ViewProj", camera->GetViewPorjMat());
	_shader->SetVec3("f_EyePos", camera->_transform._pos);

}

void ST::Renderer3D::BeginDrawSkyBox(ST_REF<Shader> shader, ST_REF<Camera> camera) {
	_shader = shader;
	_shader->UseShader();
	auto viewMat = glm::mat4(glm::mat3(camera->_viewMat));
	_shader->SetMat4("v_ViewProj", camera->_projMat*viewMat);
	_shader->SetVec3("f_EyePos", camera->_transform._pos);
}

void ST::Renderer3D::PostProcessRecordBegin() {
	_frameBuffer->Bind();
}

void ST::Renderer3D::PostProcessRecordEnd() {
	_frameBuffer->UnBind();
}

void ST::Renderer3D::BeginPostProcess() {
	_frameBuffer->BindTexture();
}

void ST::Renderer3D::DrawModel(ST_REF<Model> model, const Transform& transform) {
	glm::mat4 modelTrans(1.0);
	modelTrans = scale(modelTrans, transform._scale);
	modelTrans = mat4_cast(MathLibrary::EulerToQuat(transform._rotator)) * modelTrans;
	modelTrans = translate(modelTrans, transform._pos);
	_shader->SetMat4("v_Model", modelTrans);
	for (auto& mesh : model->_meshes) {
		DrawMesh(mesh, transform);
	}
}

void ST::Renderer3D::DrawModelByColor(ST_REF<Model> model, const Transform& transform, const glm::vec4& color) {
	glm::mat4 modelTrans(1.0);
	modelTrans = glm::translate(modelTrans, transform._pos);
	modelTrans = glm::mat4_cast(MathLibrary::EulerToQuat(transform._rotator)) * modelTrans;
	modelTrans = glm::scale(modelTrans, transform._scale);

	_shader->SetMat4("v_Model", modelTrans);
	for (auto& mesh : model->_meshes) {
		DrawMeshByColor(mesh, transform, color);
	}
}

void ST::Renderer3D::DrawMesh(ST_REF<Mesh> mesh, const Transform& transform) {
	mesh->_vertexArray->Bind();
	mesh->_vertexArray->_vertexBuffers[0]->Bind();

	for (auto& material : mesh->_materials) {
		material->Bind();
		_shader->SetMaterial("f_Material", material);
	}

	if (mesh->_indices.size() > 0) {
		glDrawElements(GL_TRIANGLES, mesh->_indices.size(),GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, mesh->_verts.size());
	}
}

void ST::Renderer3D::DrawMeshByColor(ST_REF<Mesh> mesh, const Transform& transform, const glm::vec4& color) {
	mesh->_vertexArray->Bind();
	mesh->_vertexArray->_vertexBuffers[0]->Bind();

	_shader->SetVec4("f_Color", color);

	if (mesh->_indices.size() > 0) {
		glDrawElements(GL_TRIANGLES, mesh->_indices.size(),GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, mesh->_verts.size());
	}
}

void ST::Renderer3D::DrawQuad(ST_REF<Mesh> mesh) {
	mesh->_vertexArray->Bind();
	mesh->_vertexArray->_vertexBuffers[0]->Bind();
	
	_shader->SetInt("f_Texture", 0);

	if (mesh->_indices.size() > 0) {
		glDrawElements(GL_TRIANGLES, mesh->_indices.size(),GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, mesh->_verts.size());
	}
}

void ST::Renderer3D::DrawSkyBox(ST_REF<Mesh> mesh) {
	_skyBox->Bind();
	_shader->SetInt("f_cubeMap",0);
	DrawMesh(mesh,Transform{});
}

void ST::Renderer3D::DrawLight(ST_REF<Mesh> mesh) {
	DrawMesh(mesh, Transform{_pointLight->_pos});
	_shader->SetVec3("f_LightColor", _pointLight->_ia);
}

void ST::Renderer3D::DrawGameObject(ST_REF<GameObject> gameObject) {
	DrawModel(gameObject->_model, gameObject->_transform);
}

void ST::Renderer3D::DrawScaledGameObjectByColor(ST_REF<GameObject> gameObject, const glm::vec3& scale,
	const glm::vec4& color) {
	auto& transform = gameObject->_transform;
	DrawModelByColor(gameObject->_model, Transform(transform._pos, transform._rotator, transform._scale * scale),
		color);
}
