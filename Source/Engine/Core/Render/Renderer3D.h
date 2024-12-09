#pragma once
#include "Core.h"
#include "Light.h"

namespace ST {
class CubeMap;

class FrameBuffer;

class Model;

class Mesh;

class CameraController;

class Camera;

struct Transform;

class Texture2D;

class VertexArray;

class AppWindow;

class Shader;

class GameObject;

class Renderer3D {
public:
	Renderer3D(AppWindow* window);

	void SetLight();

	void PostProcessRecordBegin();

	void PostProcessRecordEnd();

	void BeginPostProcess();

	void BeginDraw(ST_REF<Shader> shader, ST_REF<Camera> camera);
	
	void BeginDrawSkyBox(ST_REF<Shader> shader, ST_REF<Camera> camera);

	void DrawLight(ST_REF<Mesh> mesh);
	
	void DrawGameObject(ST_REF<GameObject> gameObject);

	void DrawScaledGameObjectByColor(ST_REF<GameObject> gameObject, const glm::vec3& scale, const glm::vec4& color);

	void DrawQuad(ST_REF<Mesh> mesh);

	void DrawSkyBox(ST_REF<Mesh> mesh);

	ST_REF<DirLight> _dirLight = ST_MAKE_REF<DirLight>(
		glm::vec3{0.2,0.75,1}, glm::vec3{0.5, 0.5, 0.5}, glm::vec3{0.7, 0.7, 0.7}, glm::vec3{0.7, 0.7, 0.7});

	ST_REF<PointLight> _pointLight =
		ST_MAKE_REF<PointLight>(glm::vec3{10, 0, 0}, glm::vec3{}, glm::vec3{}, glm::vec3{},
			1.0f, 0.045f, 0.0075f);

private:
	void DrawModel(ST_REF<Model> model, const Transform& transform);

	void DrawModelByColor(ST_REF<Model> model, const Transform& transform, const glm::vec4& color);

	void DrawMesh(ST_REF<Mesh> mesh, const Transform& transform);

	void DrawMeshByColor(ST_REF<Mesh> mesh, const Transform& transform, const glm::vec4& color);

	AppWindow* _window;

	ST_REF<Shader> _shader;

	ST_REF<FrameBuffer> _frameBuffer;

	ST_REF<CubeMap> _skyBox;
};
}
