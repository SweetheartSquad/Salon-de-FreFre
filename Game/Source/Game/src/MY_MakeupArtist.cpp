#pragma once

#include <MY_MakeupArtist.h>
#include <MY_ResourceManager.h>

MY_MakeupArtist::MY_MakeupArtist(Shader * _shader, std::vector<glm::vec2> _points) :
	MeshEntity(MY_ResourceManager::globalAssets->getMesh("artist")->meshes.at(0), _shader),
	points(_points),
	currentPointIdx(0)
{
	mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("artist")->texture);
	mesh->setScaleMode(GL_NEAREST);
}

void MY_MakeupArtist::update(Step * _step){
	if(firstParent() != nullptr && currentPointIdx < points.size()) {
		firstParent()->translate(points[currentPointIdx].x, firstParent()->getTranslationVector().y, points[currentPointIdx].y, false);
		currentPointIdx++;
	}
	MeshEntity::update(_step);
}