#pragma once

#include <MY_ResourceManager.h>

Scenario * MY_ResourceManager::globalAssets = nullptr;

void MY_ResourceManager::init(){
	globalAssets = new Scenario("assets/scenario.json");
	resources.push_back(globalAssets);
}