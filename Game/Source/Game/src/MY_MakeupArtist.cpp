#pragma once

#include <MY_MakeupArtist.h>
#include <MY_ResourceManager.h>

MY_MakeupArtist::MY_MakeupArtist(Shader * _shader, std::vector<glm::vec2> _points) :
	points(_points),
	currentPointIdx(0)
{
	torso	= new MeshEntity(MY_ResourceManager::globalAssets->getMesh("torso")->meshes.at(0), _shader);
	head	= new MeshEntity(MY_ResourceManager::globalAssets->getMesh("head")->meshes.at(0), _shader);
	handR	= new MeshEntity(MY_ResourceManager::globalAssets->getMesh("hand")->meshes.at(0), _shader);
	handL	= new MeshEntity(MY_ResourceManager::globalAssets->getMesh("hand")->meshes.at(0), _shader);
	footR	= new MeshEntity(MY_ResourceManager::globalAssets->getMesh("foot")->meshes.at(0), _shader);
	footL	= new MeshEntity(MY_ResourceManager::globalAssets->getMesh("foot")->meshes.at(0), _shader);
	eyeR	= new MeshEntity(MY_ResourceManager::globalAssets->getMesh("eye")->meshes.at(0), _shader);
	eyeL	= new MeshEntity(MY_ResourceManager::globalAssets->getMesh("eye")->meshes.at(0), _shader);

	std::vector<MeshEntity *> meshes;
	meshes.push_back(torso);
	meshes.push_back(head);
	meshes.push_back(handR);
	meshes.push_back(handL);
	meshes.push_back(footR);
	meshes.push_back(footL);
	meshes.push_back(eyeR);
	meshes.push_back(eyeL);

	for(auto m : meshes){
		m->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("artist")->texture);
		m->mesh->setScaleMode(GL_NEAREST);
	}

	

	childTransform->addChild(torso);

	torso->childTransform->addChild(head);
	torso->childTransform->addChild(handR);
	torso->childTransform->addChild(handL);
	torso->childTransform->addChild(footR);
	torso->childTransform->addChild(footL);
	
	head->childTransform->addChild(eyeR);
	head->childTransform->addChild(eyeL);

	torso->firstParent()->translate(glm::vec3(0, 1.672, 0));
	head->firstParent()->translate(glm::vec3(0, 1.765, -0.018));
	handR->firstParent()->translate(glm::vec3(-0.803, 0.561, 0));
	handL->firstParent()->translate(glm::vec3(0.803, 0.561, 0))->scale(glm::vec3(-1,1,1));
	footR->firstParent()->translate(glm::vec3(-0.262, -1.446, -0.216));
	footL->firstParent()->translate(glm::vec3(0.262, -1.446, -0.216))->scale(glm::vec3(-1,1,1));
	eyeR->firstParent()->translate(glm::vec3(-0.277, 0.774, 0.44));
	eyeL->firstParent()->translate(glm::vec3(0.277, 0.774, 0.44))->scale(glm::vec3(-1,1,1));
}

void MY_MakeupArtist::update(Step * _step){
	if(firstParent() != nullptr && currentPointIdx < points.size()) {
		glm::vec3 current = firstParent()->getTranslationVector();
		glm::vec2 target = points[currentPointIdx];
		glm::vec2 d = target - glm::vec2(current.x, current.z);
		if(glm::length2(d) > FLT_EPSILON){
			firstParent()->translate(glm::vec3(d.x, 0, d.y) * 0.5f);
		}
		currentPointIdx++;
	}
	Entity::update(_step);
}