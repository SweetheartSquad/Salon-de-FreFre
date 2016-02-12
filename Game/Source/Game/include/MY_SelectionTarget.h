#pragma once

#include <BulletMeshEntity.h>

class MY_SelectionTarget : public BulletMeshEntity{
public:
	std::string name;
	std::string type;
	Texture * texture;

	MY_SelectionTarget(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader);
};