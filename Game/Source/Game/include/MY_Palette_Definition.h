#pragma once

#include <string>
#include <vector>

#define NUM_OPTIONS 6

class MY_SelectionTarget;
class Texture;

class MY_PaletteOption_Definition {
public:
	std::string name;
	Texture * texture; // texture that will be applied when selected

	MY_PaletteOption_Definition(std::string _name, Texture * _texture);
	~MY_PaletteOption_Definition();
};

class MY_Palette_Definition {
public:
	std::string name;
	Texture * texture; // texture of the palette

	MY_Palette_Definition(std::string _name, Texture * _texture);
	~MY_Palette_Definition();

	std::vector<MY_PaletteOption_Definition *> options;
};