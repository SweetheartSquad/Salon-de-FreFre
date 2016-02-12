#pragma once

#include <MY_Palette_Definition.h>
#include <MY_ResourceManager.h>

MY_PaletteOption_Definition::MY_PaletteOption_Definition(std::string _name, Texture * _texture) :
	name(_name),
	texture(_texture)
{
}

MY_Palette_Definition::MY_Palette_Definition(std::string _name, Texture * _texture) :
	name(_name),
	texture(_texture)
{
	for(unsigned int i = 0; i < NUM_OPTIONS; ++i){
		options.push_back(new MY_PaletteOption_Definition(name + "_" + std::to_string(i), MY_ResourceManager::globalAssets->getTexture(name + "_" + std::to_string(i))->texture));
	}
}