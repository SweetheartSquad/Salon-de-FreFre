#pragma once

#include <MY_Scene_Base.h>
#include "ShaderComponentCircularMask.h"
#include <MY_SelectionTarget.h>
#include <MY_MakeupArtist.h>
#include <MY_Palette.h>

#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <FBOTexture.h>
#include <EventManager.h>
#include <MY_Palette_Definition.h>

class StereoCamera;
class Tracks;
class MY_Avatar;
class ShaderComponentBlur;

class MY_Scene_Main : public MY_Scene_Base{
public:
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;
	NodeUI * fade;
	bool done;

	float startTime;

	sweet::EventManager * eventManager;

	Tracks * tracks;

	// The scene's physics world
	BulletWorld * bulletWorld;
	// used to draw wireframes showing physics colliders, transforms, etc
	BulletDebugDrawer * bulletDebugDrawer;


	// ID of the currently playing audio track
	signed long int currentTrackId;
	// currently playing audio track
	OpenAL_Sound * currentTrack;
	// increments currentTrackId and replaces the current track with the one located at the new ID
	void loadNextPalette();
	void getNextTrack();
	void makeSelection();

	ComponentShaderBase * indicatorShader; 
	ShaderComponentCircularMask * maskComponentIndicator;


	ComponentShaderBase * mirrorShader;
	ShaderComponentBlur * mirrorBlur;


	StereoCamera * vrCam;
	MY_MakeupArtist * artist;

	MY_Palette * palette;

	int paletteDefIdx;
	std::vector<MY_Palette_Definition *> paletteDefs;
	
	MY_Scene_Main(Game * _game);
	~MY_Scene_Main();

	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	virtual void update(Step * _step) override;
	
	// overriden to add physics debug drawing
	virtual void enableDebug() override;
	// overriden to remove physics debug drawing
	virtual void disableDebug() override;

	
	// whether we are paused and waiting for user input in order to proceed
	bool waitingForInput;
	// updates currentHoverTarget based on raycasting
	void updateHoverTarget(Step * _step);
	MY_SelectionTarget * currentHoverTarget;
	// length of time the currenHoverTarget has been hovered
	float hoverTime;
	// length of time a target must be hovered before it is chosen as a selection
	float targetHoverTime;

	std::vector<Texture *> selections;
	
	PerspectiveCamera * mirrorCamera;
	StandardFrameBuffer * mirrorFBO;
	FBOTexture * mirrorTex;
	MeshEntity * mirrorSurface;
	
	MY_Avatar * avatar;
};