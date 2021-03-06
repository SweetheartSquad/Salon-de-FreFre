#pragma once

#include "MY_Scene_Main.h"
#include "MY_ResourceManager.h"

#include <StereoCamera.h>
#include <CubeMap.h>
#include <Sprite.h>
#include <ShaderComponentCircularMask.h>
#include <ShaderComponentBlur.h>
#include <shader/ShaderComponentTexture.h>
#include <Resource.h>
#include <VerticalLinearLayout.h>
#include <MeshFactory.h>
#include <Tracks.h>
#include <MY_Avatar.h>
#include <RenderOptions.h>

#define ROOM_WIDTH  50.f
#define ROOM_DEPTH 50.f

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
	bulletWorld(new BulletWorld(glm::vec3(0, -9.8, 0.1))),
	bulletDebugDrawer(new BulletDebugDrawer(bulletWorld->world)),
	indicatorShader(new ComponentShaderBase(true)),
	maskComponentIndicator(nullptr),
	mirrorShader(new ComponentShaderBase(true)),
	vrCam(new StereoCamera()),


	currentTrack(nullptr),
	currentTrackId(-1),

	screenSurfaceShader(new Shader("assets/engine basics/DefaultRenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader, true)),
	screenFBO(new StandardFrameBuffer(true)),
	
	paletteDefIdx(-1),
	
	waitingForInput(false),
	currentHoverTarget(nullptr),
	hoverTime(0),
	targetHoverTime(1.f),

	eventManager(new sweet::EventManager()),
	done(false)
{

	indicatorShader->addComponent(new ShaderComponentMVP(indicatorShader));	
	indicatorShader->addComponent(new ShaderComponentTexture(indicatorShader, 0.1f));
	maskComponentIndicator = new ShaderComponentCircularMask(indicatorShader, 0.1);
	indicatorShader->addComponent(maskComponentIndicator);
	indicatorShader->compileShader();
	indicatorShader->incrementReferenceCount();

	mirrorShader->addComponent(new ShaderComponentMVP(mirrorShader));
	mirrorShader->addComponent(new ShaderComponentTexture(mirrorShader, 0.1f));
	mirrorShader->addComponent(mirrorBlur = new ShaderComponentBlur(mirrorShader));
	mirrorShader->compileShader();
	mirrorShader->incrementReferenceCount();

	// Setup the debug drawer and add it to the scene
	bulletWorld->world->setDebugDrawer(bulletDebugDrawer);
	childTransform->addChild(bulletDebugDrawer, false);
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);


	MeshInterface * chairMesh = MY_ResourceManager::globalAssets->getMesh("chair")->meshes.at(0);
	chairMesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("chair")->texture);
	chairMesh->setScaleMode(GL_NEAREST);

	for(signed long int i = -1; i <= 1; ++i){
		MeshEntity * chair = new MeshEntity(chairMesh, baseShader);
		childTransform->addChild(chair)->translate(glm::vec3(i * 3, 0, 0));
	}
	
	
	std::vector<TriMesh *> environmentMeshes = MY_ResourceManager::globalAssets->getMesh("salon-environment")->meshes;
	std::vector<TriMesh *> propMeshes = MY_ResourceManager::globalAssets->getMesh("salon-props")->meshes;
	std::vector<std::string> environmentMeshOrder;
	environmentMeshOrder.push_back("floor");
	environmentMeshOrder.push_back("walls");
	environmentMeshOrder.push_back("ceiling");
	environmentMeshOrder.push_back("storefront");
	environmentMeshOrder.push_back("door");
	environmentMeshOrder.push_back("windows");
	environmentMeshOrder.push_back("road");
	environmentMeshOrder.push_back("buildings");
	environmentMeshOrder.push_back("sidewalk");

	std::vector<glm::vec3> propMeshOrder;
	propMeshOrder.push_back(glm::vec3(1, 0.96, 0));
	propMeshOrder.push_back(glm::vec3(0.8, 0.45, 0.45));
	propMeshOrder.push_back(glm::vec3(0.5, 0.05, 0.2));
	propMeshOrder.push_back(glm::vec3(1,1,1));
	propMeshOrder.push_back(glm::vec3(0.02, 0.6, 0));
	propMeshOrder.push_back(glm::vec3(0.4, 0.9, 0.9));
	
	for(unsigned long int i = 0; i < environmentMeshes.size(); ++i){
		MeshEntity * c = new MeshEntity(environmentMeshes.at(i), baseShader);
		c->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("salon-" + environmentMeshOrder.at(i))->texture);
		c->mesh->setScaleMode(GL_NEAREST);
		childTransform->addChild(c, false);
	}
	for(unsigned long int i = 0; i < propMeshes.size(); ++i){
		MeshEntity * c = new MeshEntity(propMeshes.at(i), baseShader);
		for(auto & v : c->mesh->vertices){
			v.red = propMeshOrder.at(i).r;
			v.green = propMeshOrder.at(i).g;
			v.blue = propMeshOrder.at(i).b;
		}
		c->mesh->setScaleMode(GL_NEAREST);
		childTransform->addChild(c, false);
	}


	activeCamera = vrCam;
	cameras.push_back(vrCam);
	vrCam->yaw = -90;
	vrCam->nearClip = 0.001f;
	vrCam->fieldOfView = 110;

	avatar = new MY_Avatar(baseShader, vrCam);
	childTransform->addChild(avatar);
	avatar->head->childTransform->addChild(vrCam)->translate(0, 0.5f, 0);

	/*MeshEntity * test = new MeshEntity(Resource::loadMeshFromObj("assets/meshes/buttman.obj", true).at(0), baseShader);
	Texture * tex = new Texture("assets/textures/buttman.png", false, true, true);
	tex->load();
	test->mesh->pushTexture2D(tex);
	childTransform->addChild(test)->translate(0, 3, 2)->rotate(90, 0, 1, 0, kOBJECT);*/
	

	
	/*MY_SelectionTarget * palette = new MY_SelectionTarget(bulletWorld, MY_ResourceManager::globalAssets->getMesh("palette")->meshes.at(0), baseShader);
	palette->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("palette")->texture);
	childTransform->addChild(palette);
	palette->setColliderAsBoundingBox();
	//palette->setColliderAsSphere(5);
	palette->createRigidBody(0);
	palette->translatePhysical(glm::vec3(0, 3, 2));
	palette->name = "test";*/
	//palette->rotatePhysical(90, 0, 1, 0, kOBJECT);
	
	CameraController * c = new CameraController(vrCam);
	c->movementSpeed = 0.05f;
	vrCam->childTransform->addChild(c, false);

	std::vector<glm::vec2> points;
	std::string json = sweet::FileUtils::readFile("assets/path.json");
	Json::Reader reader;
	Json::Value path;
	bool parsingSuccessful = reader.parse( json, path);
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
	}else{		
		glm::vec2 ratio = glm::vec2(ROOM_WIDTH/path["windowSize"][0].asFloat(), ROOM_DEPTH/path["windowSize"][1].asFloat());
		for(auto point : path["points"]) {
			// Reverse co-ordinates because player is facing down z axis
			float x = /*ratio.y * */point[0].asFloat();
			float z = /*ratio.x * */point[1].asFloat() / glm::length(ratio);
			points.push_back(glm::vec2(x, z));	
		}
	}

	// setup the artist
	artist = new MY_MakeupArtist(baseShader, points);
	childTransform->addChild(artist);//->scale(0.9);

	palette = new MY_Palette(bulletWorld, baseShader);
	childTransform->addChild(palette);
	palette->translateComponents(glm::vec3(0, 3, 2));

	

	// parse palettes
	{
		Json::Reader reader;
		Json::Value json;
		bool parsingSuccessful;
	
		parsingSuccessful = reader.parse(sweet::FileUtils::readFile("assets/palettes.json"), json);

		assert(parsingSuccessful);

		for(Json::Value::ArrayIndex i = 0; i < json["palettes"].size(); ++i){
			paletteDefs.push_back(new MY_Palette_Definition(json["palettes"][i].get("name", "NO_NAME").asString(), MY_ResourceManager::globalAssets->getTexture(json["palettes"][i].get("texture", "DEFAULT").asString())->texture));
		}
	}

	Sprite * sprite = new Sprite(MY_ResourceManager::globalAssets->getTexture("indicator")->texture, indicatorShader);

	auto sd = sweet::getWindowDimensions();
	uiLayer->resize(0, sd.x, 0, sd.y);

	VerticalLinearLayout * crosshairLayout = new VerticalLinearLayout(uiLayer->world);
	crosshairLayout->setRationalWidth(1.0f, uiLayer);
	crosshairLayout->setRationalHeight(1.0f, uiLayer);
	crosshairLayout->horizontalAlignment = kCENTER;
	crosshairLayout->verticalAlignment = kMIDDLE;

	NodeUI * crossHair = new NodeUI(uiLayer->world);
	crossHair->setWidth(15);
	crossHair->setHeight(15);
	crossHair->background->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("crosshair")->texture);
	crossHair->background->mesh->setScaleMode(GL_NEAREST);

	crosshairLayout->addChild(crossHair);

	crossHair->childTransform->addChild(sprite)->scale(100)->translate(7.5f, 7.5f, 0.f);

	uiLayer->addChild(crosshairLayout);

	
	// setup mirror
	mirrorCamera = new PerspectiveCamera();
	childTransform->addChild(mirrorCamera);
	cameras.push_back(mirrorCamera);
	mirrorCamera->firstParent()->translate(-0.5f, 3.25f, 4, false);

	mirrorFBO = new StandardFrameBuffer(true);
	mirrorTex = new FBOTexture(mirrorFBO, true, 0, false);
	mirrorTex->load();
	mirrorSurface = new MeshEntity(MY_ResourceManager::globalAssets->getMesh("salon-mirror")->meshes.at(0), mirrorShader);
	mirrorSurface->mesh->setScaleMode(GL_LINEAR);
	mirrorSurface->mesh->uvEdgeMode = GL_CLAMP_TO_EDGE;
	mirrorSurface->mesh->pushTexture2D(mirrorTex);
	childTransform->addChild(mirrorSurface);
	mirrorFBO->incrementReferenceCount();


	// memory management
	screenSurface->incrementReferenceCount();
	screenSurfaceShader->incrementReferenceCount();
	screenFBO->incrementReferenceCount();



	// load the tracks
	tracks = new Tracks();

	// start the experience
	getNextTrack();
	loadNextPalette();



	fade = new NodeUI(uiLayer->world);
	uiLayer->addChild(fade);
	fade->setRationalHeight(1.f, uiLayer);
	fade->setRationalWidth(1.f, uiLayer);
	fade->setBackgroundColour(0,0,0,1);

	Timeout * fadeIn = new Timeout(1.f, [this](sweet::Event * _event){
		fade->setVisible(false);
	});
	fadeIn->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		fade->setBackgroundColour(0,0,0,1.f-p);
	});
	fadeIn->start();
	childTransform->addChild(fadeIn, false);


	// recenter HMD
	ovr_RecenterPose(*sweet::hmd);

	startTime = glfwGetTime();
}

MY_Scene_Main::~MY_Scene_Main(){
	deleteChildTransform();

	// memory management
	screenSurface->decrementAndDelete();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
	mirrorFBO->decrementAndDelete();

	indicatorShader->decrementAndDelete();
	mirrorShader->decrementAndDelete();

	delete eventManager;
	delete bulletWorld;
	delete tracks;
}

void MY_Scene_Main::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->setClearColour(0.2, 0.8, 1, 1);
	// render the mirror texture
	{
		_renderOptions->setViewPort(0, 0, 1024, 1024);
		mirrorFBO->resize(_renderOptions->viewPortDimensions.width, _renderOptions->viewPortDimensions.height);
		FrameBufferInterface::pushFbo(mirrorFBO);
		Camera * c = activeCamera;

		activeCamera = mirrorCamera;
		avatar->head->setVisible(true);
		uiLayer->setVisible(false);
		mirrorSurface->setVisible(false);
		palette->setVisible(false);
		MY_Scene_Base::render(_matrixStack, _renderOptions);

		activeCamera = c;
		FrameBufferInterface::popFbo();
		// update the mirror texture
		mirrorTex->refresh();
	}

	// render the scene
	vrCam->renderStereo([this, _matrixStack, _renderOptions](){

		
		// keep our screen framebuffer up-to-date with the game's viewport
		glm::uvec2 sd = sweet::getWindowDimensions();
		_renderOptions->setViewPort(0, 0, sd.x, sd.y);
		screenFBO->resize(_renderOptions->viewPortDimensions.width, _renderOptions->viewPortDimensions.height);

		// bind our screen framebuffer
		FrameBufferInterface::pushFbo(screenFBO);
		// render the scene
		avatar->head->setVisible(false);
		uiLayer->setVisible(true);
		mirrorSurface->setVisible(true);
		palette->setVisible(true);
		MY_Scene_Base::render(_matrixStack, _renderOptions);
		// unbind our screen framebuffer, rebinding the previously bound framebuffer
		// since we didn't have one bound before, this will be the default framebuffer (i.e. the one visible to the player)
		FrameBufferInterface::popFbo();

		// render our screen framebuffer using the standard render surface
		screenSurface->render(screenFBO->getTextureId());

		// render the uiLayer after the screen surface in order to avoid hiding it through shader code
		uiLayer->render(_matrixStack, _renderOptions);
	});
	// If an hmd is connected, we blit the stereo camera's buffers back to the screen after we're done rendering
	if(sweet::ovrInitialized){
		//vrCam->blitTo(0);
	}

	//uiLayer->render(_matrixStack, _renderOptions);
}

void MY_Scene_Main::update(Step * _step){
	maskComponentIndicator->setRatio(hoverTime/targetHoverTime);

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
				makeSelection();
				getNextTrack();
			}
		}
	}else{
		// update the active audio stream
		currentTrack->update(_step);

		// update the position of the active audio stream
		// TODO

		// if the audio stream has finished, switch to user input
		if(currentTrack->source->state != AL_PLAYING && glfwGetTime() - startTime > 2.f){
			if(tracks->tracks.at(currentTrackId).needsInput){
				waitingForInput = true;
			}else{
				getNextTrack();
			}
		}
	}

	// update the listener's position/orientation based on the camera
	NodeOpenAL::setListener(vrCam, true);

	// update the orientation of the artist
	float d = artist->head->childTransform->getWorldPos().y - artist->childTransform->getWorldPos().y;
	artist->childTransform->lookAt(activeCamera->childTransform->getWorldPos() - glm::vec3(0, 3.25, 0), glm::vec3(0, 1, 0), 0.05f);
	artist->paused = waitingForInput;
	if(done){
		artist->paused = true;
	}

	// update the physics bodies
	bulletWorld->update(_step);

	// update the scene
	MY_Scene_Base::update(_step);

	auto sd = sweet::getWindowDimensions();
	uiLayer->resize(0, sweet::ovrInitialized ? sd.x/2 : sd.x, 0, sd.y);
	uiLayer->update(_step);

	// update the mirror
	// (it's important to do this after the scene update, because we're overriding attributes which the camera typically handles on its own)
	mirrorCamera->forwardVectorRotated = glm::vec3(0,0, -1);//glm::reflect(glm::normalize(activeCamera->forwardVectorRotated * glm::vec3(1,0,1)), glm::normalize(mirrorCamera->childTransform->getWorldPos() - activeCamera->childTransform->getWorldPos()));
	mirrorCamera->lookAtSpot = mirrorCamera->lookFromSpot + mirrorCamera->forwardVectorRotated;

	mirrorBlur->blurAmount += ((float)currentTrackId / tracks->tracks.size() - mirrorBlur->blurAmount) * 0.01f;
}

void MY_Scene_Main::loadNextPalette(){
	++paletteDefIdx;
	if(paletteDefIdx < paletteDefs.size()){
		palette->loadDefinition(paletteDefs.at(paletteDefIdx));
	}else{
		eventManager->triggerEvent("palettesComplete");
	}
}

void MY_Scene_Main::makeSelection(){
	selections.push_back(currentHoverTarget->texture);

	// update the avatar mesh piece with the id "data" to the texture "data"+"target"
	avatar->meshPieces[tracks->tracks.at(currentTrackId).data]->replaceTextures(MY_ResourceManager::globalAssets->getTexture(currentHoverTarget->name)->texture);

	// special case for eyeliner (we need to unhide the lashes and liner meshes)
	if (tracks->tracks.at(currentTrackId).data == "eyeliner"){
		avatar->lashes->setVisible(true);
		avatar->liner->setVisible(true);
	}

	loadNextPalette();
}


void MY_Scene_Main::getNextTrack(){
	
	if(currentTrackId < 5){
		// stop the old track and remove it from the scene
		if(currentTrack != nullptr){
			currentTrack->stop();
			artist->head->childTransform->removeChild(currentTrack);
		}

		++currentTrackId;
		currentTrack = MY_ResourceManager::globalAssets->getAudio(tracks->tracks.at(currentTrackId).audioTrack)->sound;
	
		// add the new track to the scene and play it
		artist->head->childTransform->addChild(currentTrack, false);
		currentTrack->play(); // (shouldn't actually loop in the final, just for testing)

		// reset the selection stuff
		waitingForInput = false;
		currentHoverTarget = nullptr;
		hoverTime = 0;
	}else if(!done){
		done = true;
	}
}


void MY_Scene_Main::updateHoverTarget(Step * _step){
	// if we're not looking at anything, cancel the current hover target
	NodeBulletBody * me = bulletWorld->raycast(activeCamera, 5);
	if(me == nullptr){
		currentHoverTarget = nullptr;
		hoverTime = 0;
		return;
	}

	// if what we're looking at isn't a valid target, cancel the current hover target
	MY_SelectionTarget * potentialTarget = dynamic_cast<MY_SelectionTarget *>(me);
	if(potentialTarget == nullptr){
		currentHoverTarget = nullptr;
		hoverTime = 0;
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