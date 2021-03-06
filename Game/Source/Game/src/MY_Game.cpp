#pragma once

#include <MY_Game.h>

#include <MY_ResourceManager.h>

#include <MY_Scene_Menu.h>


MY_Game::MY_Game() :
	Game("menu", new MY_Scene_Menu(this), true) // initialize our game with a menu scene
{
	// initialize all of our scenes in the game's scene map for later use
	// only the current scene is updated/rendered by default, so these shouldn't affect performance
	// however, they will be created at the same time as the game, so any load times associated with
	// their creation will be front-loaded. To avoid this, you can check to see if a scene exists immediately
	// before switching to it, and create it then if needed.
	//scenes["vr"] = new MY_Scene_VR(this);
}

MY_Game::~MY_Game(){}

void MY_Game::addSplashes(){
	// add default splashes
	//Game::addSplashes();

	// add custom splashes
	//addSplash(new Scene_Splash(this, MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture, MY_ResourceManager::globalAssets->getAudio("DEFAULT")->sound));
}