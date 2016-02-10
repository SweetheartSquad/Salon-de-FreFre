#pragma once

#include "MY_Scene_Main.h"
#include "MY_ResourceManager.h"

#include <StereoCamera.h>
#include <CubeMap.h>
#include <Resource.h>

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
	vrCam(new StereoCamera()),

	currentHoverTarget(nullptr),
	hoverTime(0),
	targetHoverTime(1.5f),
	waitingForInput(true),

	bulletWorld(new BulletWorld(glm::vec3(0, -9.8, 0.1))), // we initialize the world's gravity here
	bulletDebugDrawer(new BulletDebugDrawer(bulletWorld->world)),

	currentTrack(nullptr),
	currentTrackId(0)
{
	// Setup the debug drawer and add it to the scene
	bulletWorld->world->setDebugDrawer(bulletDebugDrawer);
	childTransform->addChild(bulletDebugDrawer, false);
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);


	MeshInterface * chairMesh = MY_ResourceManager::globalAssets->getMesh("chair")->meshes.at(0);
	chairMesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("chair")->texture);
	
	MeshEntity * chair = new MeshEntity(chairMesh, baseShader);

	childTransform->addChild(chair);
	
	childTransform->addChild(vrCam)->translate(0, 4, 0);
	activeCamera = vrCam;
	cameras.push_back(vrCam);
	vrCam->yaw = -90;



	/*MeshEntity * test = new MeshEntity(Resource::loadMeshFromObj("assets/meshes/buttman.obj", true).at(0), baseShader);
	Texture * tex = new Texture("assets/textures/buttman.png", false, true, true);
	tex->load();
	test->mesh->pushTexture2D(tex);
	childTransform->addChild(test)->translate(0, 3, 2)->rotate(90, 0, 1, 0, kOBJECT);*/
	

	
	MY_SelectionTarget * palette = new MY_SelectionTarget(bulletWorld, MY_ResourceManager::globalAssets->getMesh("palette")->meshes.at(0), baseShader);
	palette->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("palette")->texture);
	childTransform->addChild(palette);
	palette->setColliderAsBoundingBox();
	//palette->setColliderAsSphere(5);
	palette->createRigidBody(0);
	palette->translatePhysical(glm::vec3(0, 3, 2));
	palette->name = "test";
	//palette->rotatePhysical(90, 0, 1, 0, kOBJECT);
	

	//CameraController * c = new CameraController(vrCam);
	//vrCam->childTransform->addChild(c, false);

	// add a cubemap (cubemaps use a special texture type and shader component. these can be instantiated separately if desired, but the CubeMap class handles them both for us)
	CubeMap * cubemap = new CubeMap("assets/textures/cubemap", "png");
	childTransform->addChild(cubemap);

	// setup the artist
	artist = new MY_MakeupArtist(baseShader);

	// start the experience
	getNextTrack();
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
	if(waitingForInput){
		updateHoverTarget(_step);

		if(currentHoverTarget != nullptr){
			std::stringstream ss;
			ss << "Target: " << currentHoverTarget->name << ", Time: " << hoverTime << "/" << targetHoverTime;
			Log::info(ss.str());
			// check for selection
			if(hoverTime >= targetHoverTime){
				// make a selection and move on to the next track
				// TODO
				getNextTrack();
			}
		}
	}else{
		// update the active audio stream
		currentTrack->update(_step);

		// update the position of the active audio stream
		// TODO

		// if the audio stream has finished, switch to user input
		if(currentTrack->source->state != AL_PLAYING){
			waitingForInput = true;
		}
	}

	// update the listener's position/orientation based on the camera
	NodeOpenAL::setListener(vrCam, true);

	// update the physics bodies
	bulletWorld->update(_step);

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
	currentTrack->play(); // (shouldn't actually loop in the final, just for testing)

	// reset the selection stuff
	waitingForInput = false;
	currentHoverTarget = nullptr;
	hoverTime = 0;
}


void MY_Scene_Main::updateHoverTarget(Step * _step){
	// if we're not looking at anything, cancel the current hover target
	NodeBulletBody * me = bulletWorld->raycast(activeCamera, 5);
	if(me == nullptr){
		currentHoverTarget = nullptr;
		return;
	}

	// if what we're looking at isn't a valid target, cancel the current hover target
	MY_SelectionTarget * potentialTarget = dynamic_cast<MY_SelectionTarget *>(me);
	if(potentialTarget == nullptr){
		currentHoverTarget = nullptr;
		return;
	}

	// if we're looking at the same target as before, increment towards selection
	if(potentialTarget == currentHoverTarget && currentHoverTarget != nullptr){
		hoverTime += _step->deltaTime;
		return;
	}

	// if we're looking at a new target, restart
	currentHoverTarget = potentialTarget;
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