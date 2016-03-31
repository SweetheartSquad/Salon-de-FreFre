#pragma once

#include <MY_Avatar.h>
#include <MY_ResourceManager.h>

#include <Camera.h>

MY_Avatar::MY_Avatar(Shader * _shader, Camera * _playerCam) :
	playerCam(_playerCam)
{

	std::vector<TriMesh *> meshes = MY_ResourceManager::globalAssets->getMesh("avatarHead")->meshes;
	
	base = new MeshEntity(MY_ResourceManager::globalAssets->getMesh("avatarBase")->meshes.at(0), _shader);
	hat = new MeshEntity(meshes.at(0), _shader);
	head = new MeshEntity(meshes.at(1), _shader);
	cheeks = new MeshEntity(meshes.at(2), _shader);
	eyes = new MeshEntity(meshes.at(3), _shader);
	lashes = new MeshEntity(meshes.at(4), _shader);
	liner = new MeshEntity(meshes.at(5), _shader);
	lips = new MeshEntity(meshes.at(6), _shader);

	hat->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("fabric")->texture); 
	base->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("mascara")->texture);
	head->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("original")->texture);
	lashes->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("mascara")->texture);
	liner->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("original")->texture);
	eyes->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("original")->texture);
	cheeks->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("original")->texture);
	lips->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("original")->texture);
	
	base->mesh->setScaleMode(GL_NEAREST);
	lashes->mesh->setScaleMode(GL_NEAREST);
	liner->mesh->setScaleMode(GL_NEAREST);

	childTransform->addChild(base);

	base->childTransform->addChild(head)->translate(0, 3, 0.01);
	
	head->childTransform->addChild(hat);
	head->childTransform->addChild(cheeks);
	head->childTransform->addChild(eyes);
	head->childTransform->addChild(lashes);
	head->childTransform->addChild(liner);
	head->childTransform->addChild(lips);

	meshPieces["blush"] = cheeks->mesh;
	meshPieces["eyeshadow"] = eyes->mesh;
	meshPieces["eyeliner"] = liner->mesh;
	meshPieces["lipstick"] = lips->mesh;

	lashes->setVisible(false);
	liner->setVisible(false);
}

MY_Avatar::~MY_Avatar(){

}

void MY_Avatar::update(Step * _step){
	head->childTransform->setOrientation(playerCam->childTransform->getOrientationQuat() * glm::angleAxis(180.f, glm::vec3(0, 1, 0)));

	Entity::update(_step);
}