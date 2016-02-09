#pragma once

#include "MY_Scene_Main.h"
#include "MY_ResourceManager.h"

#include <StereoCamera.h>
#include <CubeMap.h>

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
	vrCam(new StereoCamera())
{
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