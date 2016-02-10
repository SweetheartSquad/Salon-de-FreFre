#pragma once

#include <MY_SelectionTarget.h>

class MY_Palette : public Entity{
private:
	MeshEntity * base;
	std::vector<MY_SelectionTarget *> options;
public:
	MY_Palette(BulletWorld * _world, Shader * _shader);


	void translateComponents(glm::vec3 _t, bool _relative = true);
};