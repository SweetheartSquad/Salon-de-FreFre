#pragma once

#include "MY_Scene_Main.h"
#include "MY_ResourceManager.h"

#include <StereoCamera.h>
#include <CubeMap.h>
#include <Sprite.h>
#include <ShaderComponentCircularMask.h>
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

	eventManager(new sweet::EventManager())
{

	indicatorShader->addComponent(new ShaderComponentMVP(indicatorShader));	
	indicatorShader->addComponent(new ShaderComponentTexture(indicatorShader, 0.1f));
	maskComponentIndicator = new ShaderComponentCircularMask(indicatorShader, 0.1);
	indicatorShader->addComponent(maskComponentIndicator);
	indicatorShader->compileShader();

	// Setup the debug drawer and add it to the scene
	bulletWorld->world->setDebugDrawer(bulletDebugDrawer);
	childTransform->addChild(bulletDebugDrawer, false);
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);


	MeshInterface * chairMesh = MY_ResourceManager::globalAssets->getMesh("chair")->meshes.at(0);
	chairMesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("chair")->texture);

	for(signed long int i = -1; i < 3; ++i){
		MeshEntity * chair = new MeshEntity(chairMesh, baseShader);
		childTransform->addChild(chair)->translate(glm::vec3(i * 3, 0, 0));
	}
	

	std::vector<TriMesh *> salonMeshes = MY_ResourceManager::globalAssets->getMesh("salon")->meshes;
	std::vector<std::string> meshOrder;
	meshOrder.push_back("floor");
	meshOrder.push_back("walls");
	meshOrder.push_back("ceiling");
	meshOrder.push_back("storefront");
	meshOrder.push_back("door");
	meshOrder.push_back("windows");
	meshOrder.push_back("road");
	meshOrder.push_back("buildings");
	meshOrder.push_back("sidewalk");

	for(unsigned long int i = 0; i < salonMeshes.size(); ++i){
		MeshEntity * c = new MeshEntity(salonMeshes.at(i), baseShader);
		c->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("salon-" + meshOrder.at(i))->texture);
		c->mesh->setScaleMode(GL_NEAREST);
		childTransform->addChild(c, false);
	}


	activeCamera = vrCam;
	cameras.push_back(vrCam);
	vrCam->yaw = -90;
	vrCam->nearClip = 0.001f;

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
	childTransform->addChild(artist);

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


	// add a cubemap (cubemaps use a special texture type and shader component. these can be instantiated separately if desired, but the CubeMap class handles them both for us)
	CubeMap * cubemap = new CubeMap("assets/textures/cubemap", "png");
	childTransform->addChild(cubemap);

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

	crosshairLayout->addChild(crossHair);

	crossHair->childTransform->addChild(sprite)->scale(100)->translate(7.5f, 7.5f, 0.f);

	uiLayer->addChild(crosshairLayout);

	
	// setup mirror
	mirrorCamera = new PerspectiveCamera();
	childTransform->addChild(mirrorCamera);
	cameras.push_back(mirrorCamera);
	mirrorCamera->firstParent()->translate(0, 5, 5, false);

	mirrorFBO = new StandardFrameBuffer(true);
	mirrorTex = new FBOTexture(mirrorFBO, true, 0, true);
	mirrorTex->load();
	mirrorSurface = new MeshEntity(MeshFactory::getPlaneMesh(), baseShader);
	childTransform->addChild(mirrorSurface)->translate(0, 5, 5, false)->scale(glm::vec3(16,-9,1));
	mirrorSurface->mesh->pushTexture2D(mirrorTex);
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
}

MY_Scene_Main::~MY_Scene_Main(){
	deleteChildTransform();

	// memory management
	screenSurface->decrementAndDelete();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
	mirrorFBO->decrementAndDelete();

	delete eventManager;
	delete bulletWorld;
	delete tracks;
}

void MY_Scene_Main::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	// render the mirror texture
	{
		FrameBufferInterface::pushFbo(mirrorFBO);
		Camera * c = activeCamera;

		activeCamera = mirrorCamera;
		avatar->head->setVisible(true);
		uiLayer->setVisible(false);
		MY_Scene_Base::render(_matrixStack, _renderOptions);

		activeCamera = c;
		FrameBufferInterface::popFbo();
	}

	// render the scene
	vrCam->renderStereo([this, _matrixStack, _renderOptions](){

		
		// keep our screen framebuffer up-to-date with the game's viewport
		screenFBO->resize(_renderOptions->viewPortDimensions.width, _renderOptions->viewPortDimensions.height);

		// bind our screen framebuffer
		FrameBufferInterface::pushFbo(screenFBO);
		// render the scene
		avatar->head->setVisible(false);
		uiLayer->setVisible(true);
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
		vrCam->blitTo(0);
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
		if(currentTrack->source->state != AL_PLAYING){
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
	glm::vec3 d = activeCamera->childTransform->getWorldPos() - artist->childTransform->getWorldPos();
	artist->childTransform->setOrientation(glm::angleAxis(glm::degrees(atan2(d.x, d.z)), glm::vec3(0,1,0)));
	artist->paused = waitingForInput;

	// update the physics bodies
	bulletWorld->update(_step);

	// update the scene
	MY_Scene_Base::update(_step);

	auto sd = sweet::getWindowDimensions();
	uiLayer->resize(0, sweet::ovrInitialized ? sd.x/2 : sd.x, 0, sd.y);
	uiLayer->update(_step);

	// update the mirror
	// (it's important to do this after the scene update, because we're overriding attributes which the camera typically handles on its own)
	mirrorCamera->forwardVectorRotated = glm::reflect(activeCamera->forwardVectorRotated, glm::normalize(mirrorCamera->childTransform->getWorldPos() - activeCamera->childTransform->getWorldPos()));
	mirrorCamera->lookAtSpot = mirrorCamera->lookFromSpot + mirrorCamera->forwardVectorRotated;
	// update the mirror texture
	mirrorTex->refresh();
	mirrorTex->bufferData();
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

	sweet::Event * e = new sweet::Event("selectionMade");
	e->setStringData("selection", currentHoverTarget->name); // the selection
	e->setStringData("type", palette->name); // sounds/generic animations for types of makeup? I don't know, powder?
	eventManager->triggerEvent(e);

	loadNextPalette();
}


void MY_Scene_Main::getNextTrack(){
	
	if(currentTrackId < 10){
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