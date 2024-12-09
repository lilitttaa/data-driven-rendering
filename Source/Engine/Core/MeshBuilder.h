#pragma once
#include "Core.h"

namespace ST {
class Mesh;

class MeshBuilder {
public:
	static ST_REF<Mesh> CreateCube();

	static ST_REF<Mesh> CreatePlane();

	static ST_REF<Mesh> CreateQuad();
};
}
