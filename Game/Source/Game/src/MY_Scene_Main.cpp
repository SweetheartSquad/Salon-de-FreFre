#pragma once

#include "MY_Scene_Main.h"
#include "MY_ResourceManager.h"

#include <StereoCamera.h>
#include <CubeMap.h>
#include <Sprite.h>
#include <ShaderComponentCircularMask.h>
#include <shader/ShaderComponentTexture.h>

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
	vrCam(new StereoCamera()),
	indicatorShader(new ComponentShaderBase(true)),
	maskComponentIndicator(nullptr),

	currentHoverTarget(nullptr),
	hoverTime(0),
	targetHoverTime(1.5f),

	bulletWorld(new BulletWorld(glm::vec3(0, -9.8, 0))), // we initialize the world's gravity here
	bulletDebugDrawer(new BulletDebugDrawer(bulletWorld->world)),

	currentTrack(nullptr),
	currentTrackId(0)
{

	indicatorShader->addComponent(new ShaderComponentMVP(indicatorShader));	
	indicatorShader->addComponent(new ShaderComponentTexture(indicatorShader));
	maskComponentIndicator = new ShaderComponentCircularMask(indicatorShader, 0.1);
	indicatorShader->addComponent(maskComponentIndicator);
	indicatorShader->compileShader();

	MeshInterface * chairMesh = MY_ResourceManager::globalAssets->getMesh("chair")->meshes.at(0);
	chairMesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("chair")->texture);
	
	MeshEntity * chair = new MeshEntity(chairMesh, baseShader);

	childTransform->addChild(chair);
	
	childTransform->addChild(vrCam);
	activeCamera = vrCam;
	cameras.push_back(vrCam);

	CameraController * c = new CameraController(vrCam);
	vrCam->childTransform->addChild(c, false);

	// add a cubemap (cubemaps use a special texture type and shader component. these can be instantiated separately if desired, but the CubeMap class handles them both for us)
	CubeMap * cubemap = new CubeMap("assets/textures/cubemap", "png");
	childTransform->addChild(cubemap);


	// start the experience
	getNextTrack();

	Sprite * sprite = new Sprite(MY_ResourceManager::globalAssets->getTexture("indicator")->texture, indicatorShader);
	childTransform->addChild(sprite)->scale(100);
}

void MY_Scene_Main::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	vrCam->renderStereo([this, _matrixStack, _renderOptions](){
		MY_Scene_Base::render(_matrixStack, _renderOptions);
	});
	// If an hmd is connected, we blit the stereo camera's buffers back to the screen after we're done rendering
	if(sweet::ovrInitialized){
		vrCam->blitTo(0);
	}
}

void MY_Scene_Main::update(Step * _step){

	maskComponentIndicator->setAngle(maskComponentIndicator->getRatio() + 0.0001f);

	if(waitingForInput){
		updateHoverTarget(_step);

		// check for selection
		if(hoverTime >= targetHoverTime){
			// make a selection and move on to the next track
			// TODO
			getNextTrack();
		}
	}else{
		// update the active audio stream
		currentTrack->update(_step);

		// update the position of the active audio stream
		// TODO
	}

	// update the listener's position/orientation based on the camera
	NodeOpenAL::setListener(vrCam, true);

	// update the scene
	MY_Scene_Base::update(_step);
}


void MY_Scene_Main::getNextTrack(){
	// stop the old track and remove it from the scene
	if(currentTrack != nullptr){
		currentTrack->stop();
		childTransform->removeChild(currentTrack);
	}

	++currentTrackId;
	std::stringstream ss;
	ss << "track" << currentTrackId;
	currentTrack = MY_ResourceManager::globalAssets->getAudio(ss.str())->sound;
	
	// add the new track to the scene and play it
	childTransform->addChild(currentTrack, false);
	currentTrack->play(true); // (shouldn't actually loop in the final, just for testing)
}


void MY_Scene_Main::updateHoverTarget(Step * _step){
	float range = 100;
	glm::vec3 pos = activeCamera->childTransform->getWorldPos();
	btVector3 start(pos.x, pos.y, pos.z);
	btVector3 dir(activeCamera->forwardVectorRotated.x, activeCamera->forwardVectorRotated.y, activeCamera->forwardVectorRotated.z);
	btVector3 end = start + dir*range;
	btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
	bulletWorld->world->rayTest(start, end, RayCallback);
		
		
	NodeBulletBody * lastHoverTarget = currentHoverTarget;
	
	// if we're not looking at anything, cancel the current hover target
	if(!RayCallback.hasHit()){
		currentHoverTarget = nullptr;
		return;
	}

	// if what we're looking at isn't a valid target, cancel the current hover target
	NodeBulletBody * me = static_cast<NodeBulletBody *>(RayCallback.m_collisionObject->getUserPointer());
	if(me != nullptr){
		currentHoverTarget = nullptr;
		return;
	}

	// if we're looking at the same target as before, increment towards selection
	if(currentHoverTarget == lastHoverTarget && lastHoverTarget != nullptr){
		hoverTime += _step->deltaTime;
		return;
	}

	// if we're looking at a new target, restart
	currentHoverTarget = me;
	hoverTime = 0;
}



void MY_Scene_Main::enableDebug(){
	MY_Scene_Base::enableDebug();
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
	childTransform->addChildAtIndex(bulletDebugDrawer, -1, false); // make sure the debug drawer is the last thing drawn
}
void MY_Scene_Main::disableDebug(){
	MY_Scene_Base::disableDebug();
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
}