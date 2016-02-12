#pragma once

#include <MY_SelectionTarget.h>

MY_SelectionTarget::MY_SelectionTarget(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader) :
	BulletMeshEntity(_world, _mesh, _shader),
	name(""),
	texture(nullptr)
{
}

