#pragma once

#include <MY_MakeupArtist.h>
#include <MY_ResourceManager.h>
#include <NumberUtils.h>

MY_MakeupArtist::MY_MakeupArtist(Shader * _shader, std::vector<glm::vec2> _points) :
	points(_points),
	currentPointIdx(0),
	angle(0),
	radius(1),
	paused(true)
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

	// blink
	blink = 1;
	blinkAnim = new Animation<float>(&blink);
	blinkAnim->tweens.push_back(new Tween<float>(5, 0, Easing::kLINEAR));
	blinkAnim->tweens.push_back(new Tween<float>(0.15, -1, Easing::kEASE_IN_CUBIC));
	blinkAnim->tweens.push_back(new Tween<float>(0.125, 1, Easing::kEASE_OUT_CUBIC));
	blinkAnim->hasStart = true;

	// breathing
	breathe = 1;
	breatheAnim = new Animation<float>(&breathe);
	float torsoScaleDelta = 0.05f;
	breatheAnim->tweens.push_back(new Tween<float>(3, torsoScaleDelta, Easing::kEASE_IN_OUT_CUBIC));
	breatheAnim->tweens.push_back(new Tween<float>(2, 0, Easing::kLINEAR));
	breatheAnim->tweens.push_back(new Tween<float>(1, -torsoScaleDelta, Easing::kEASE_IN_OUT_CUBIC));
	breatheAnim->hasStart = true;
}

MY_MakeupArtist::~MY_MakeupArtist(){
	delete breatheAnim;
	delete blinkAnim;
}

void MY_MakeupArtist::update(Step * _step){
	//ambient animation
	breatheAnim->update(_step);
	blinkAnim->update(_step);
	
	childTransform->scale(glm::vec3(1, breathe, 1), false);
	eyeR->childTransform->scale(glm::vec3(1, blink, 1), false);
	eyeL->childTransform->scale(glm::vec3(1, blink, 1), false);


	if(firstParent() != nullptr && currentPointIdx < points.size()) {
		/*glm::vec3 current = firstParent()->getTranslationVector();
		glm::vec2 target = points[currentPointIdx];
		glm::vec2 d = target - glm::vec2(current.x, current.z);
		if(glm::length2(d) > FLT_EPSILON){
			firstParent()->translate(glm::vec3(d.x, 0, d.y) * 0.5f);
		}*/
		
		float newAngle = points[currentPointIdx].x;
		float newRadius = glm::max(points[currentPointIdx].y, 0.75f);
		
		// stand off to the side when waiting
		if(paused){
			newAngle = -75;
			newRadius = 1.f;
		}

		float dAngle = newAngle - angle;
		
		while(dAngle > 360){
			dAngle -= 360;
		}while(dAngle < -360){
			dAngle += 360;
		}

		if(glm::abs(dAngle) > FLT_EPSILON){
			angle += dAngle * 0.025f;
		}float dRadius = newRadius - radius;
		if(glm::abs(dRadius) > FLT_EPSILON){
			radius += dRadius * 0.025f;
		}

		while(angle > 360){
			angle -= 360;
		}while(angle < -180){
			angle += 360;
		}

		glm::vec2 target(
			sin(glm::radians(angle)) * radius,
			cos(glm::radians(angle)) * radius
		);
		
		firstParent()->translate(glm::vec3(target.x, 0, target.y), false);

		if(!paused){
			currentPointIdx++;
		}
	}
	Entity::update(_step);
}