#include "MeshBuilder.h"

#include "vec3.hpp"
#include "Render/Material.h"
#include "Render/Mesh.h"

ST::ST_REF<ST::Mesh> ST::MeshBuilder::CreateCube() {
	return ST_MAKE_REF<ST::Mesh>(ST::ST_VECTOR<ST::Vertex>{
#pragma region /** Cube mesh */
		{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec2{0.0f, 0.0f}},
		{glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec2{1.0f, 0.0f}},
		{glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec2{1.0f, 1.0f}},
		{glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec2{1.0f, 1.0f}},
		{glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec2{0.0f, 1.0f}},
		{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec2{0.0f, 0.0f}},
	
		{glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 0.0f}},
		{glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{1.0f, 0.0f}},
		{glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{1.0f, 1.0f}},
		{glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{1.0f, 1.0f}},
		{glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 1.0f}},
		{glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 0.0f}},

		{glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{-1.0f, 0.0f, 0.0f},glm::vec2{1.0f, 0.0f}},
		{glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{-1.0f, 0.0f, 0.0f},glm::vec2{1.0f, 1.0f}},
		{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{-1.0f, 0.0f, 0.0f},glm::vec2{0.0f, 1.0f}},
		{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{-1.0f, 0.0f, 0.0f},glm::vec2{0.0f, 1.0f}},
		{glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{-1.0f, 0.0f, 0.0f},glm::vec2{0.0f, 0.0f}},
		{glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{-1.0f, 0.0f, 0.0f},glm::vec2{1.0f, 0.0f}},

		{glm::vec3{0.5f,  0.5f,  0.5f,}, glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}},
		{glm::vec3{0.5f,  0.5f, -0.5f,}, glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 1.0f}},
		{glm::vec3{0.5f, -0.5f, -0.5f,}, glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec2{0.0f, 1.0f}},
		{glm::vec3{0.5f, -0.5f, -0.5f,}, glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec2{0.0f, 1.0f}},
		{glm::vec3{0.5f, -0.5f,  0.5f,}, glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec2{0.0f, 0.0f}},
		{glm::vec3{0.5f,  0.5f,  0.5f,}, glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}},

		{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{0.0f, -1.0f, 0.0f},glm::vec2{ 0.0f, 1.0f}},
		{glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{0.0f, -1.0f, 0.0f},glm::vec2{ 1.0f, 1.0f}},
		{glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{0.0f, -1.0f, 0.0f},glm::vec2{ 1.0f, 0.0f}},
		{glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{0.0f, -1.0f, 0.0f},glm::vec2{ 1.0f, 0.0f}},
		{glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{0.0f, -1.0f, 0.0f},glm::vec2{ 0.0f, 0.0f}},
		{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{0.0f, -1.0f, 0.0f},glm::vec2{ 0.0f, 1.0f}},

		{glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{0.0f, 1.0f}},
		{glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}},
		{glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{1.0f, 0.0f}},
		{glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{1.0f, 0.0f}},
		{glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{0.0f, 0.0f}},
		{glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{0.0f, 1.0f}}
#pragma endregion
		}, ST::ST_VECTOR<unsigned int>(), ST::ST_VECTOR<ST_REF<Material>>{ST_MAKE_REF<Material>()}
		);
}

ST::ST_REF<ST::Mesh> ST::MeshBuilder::CreatePlane() {
	return ST_MAKE_REF<ST::Mesh>(ST::ST_VECTOR<ST::Vertex>{
#pragma region /** Plane mesh */
		{glm::vec3{5.0f, -0.5f,  5.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{2.0f, 0.0f}},
		{glm::vec3{ -5.0f, -0.5f,  5.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{0.0f, 0.0f}},
		{glm::vec3{-5.0f, -0.5f, -5.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{0.0f, 2.0f}},
		{glm::vec3{5.0f, -0.5f,  5.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{2.0f, 0.0f}},
		{glm::vec3{-5.0f, -0.5f, -5.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{0.0f, 2.0f}},
		{glm::vec3{5.0f, -0.5f, -5.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{2.0f, 2.0f}},
#pragma endregion
		}, ST::ST_VECTOR<unsigned int>(), ST::ST_VECTOR<ST_REF<Material>>{ST_MAKE_REF<Material>()}
		);
}

ST::ST_REF<ST::Mesh> ST::MeshBuilder::CreateQuad() {
	return ST_MAKE_REF<Mesh>(ST::ST_VECTOR<Vertex>{
#pragma region /** Quad mesh */
	{glm::vec3{-1.0f,  1.0f,0.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{ 0.0f, 1.0f}},
	{glm::vec3{-1.0f, -1.0f,0.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{ 0.0f, 0.0f}},
	{glm::vec3{ 1.0f, -1.0f,0.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{ 1.0f, 0.0f}},
	{glm::vec3{-1.0f,  1.0f,0.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{0.0f, 1.0f}},
	{glm::vec3{ 1.0f, -1.0f,0.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{1.0f, 0.0f}},
	{glm::vec3{ 1.0f,  1.0f,0.0f},glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{1.0f, 1.0f}},
#pragma endregion
	}, ST::ST_VECTOR<unsigned int>(), ST::ST_VECTOR<ST_REF<Material>>{ST_MAKE_REF<Material>()}
	);
}
