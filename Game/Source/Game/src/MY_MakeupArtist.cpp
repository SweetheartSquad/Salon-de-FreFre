#pragma once

#include <MY_MakeupArtist.h>
#include <MY_ResourceManager.h>

MY_MakeupArtist::MY_MakeupArtist(Shader * _shader) :
	MeshEntity(MY_ResourceManager::globalAssets->getMesh("artist")->meshes.at(0), _shader)
{
	mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("artist")->texture);
}

void MY_MakeupArtist::update(Step * _step){


	MeshEntity::update(_step);
}