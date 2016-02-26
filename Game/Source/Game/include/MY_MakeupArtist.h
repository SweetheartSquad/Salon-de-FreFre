#pragma once

#include <MeshEntity.h>

class MY_MakeupArtist : public Entity{
public:
	std::vector<glm::vec2> points;
	int currentPointIdx;

	MeshEntity
		* torso,
		* head,
		* handR,
		* handL,
		* footR,
		* footL,
		* eyeR,
		* eyeL;

	MY_MakeupArtist(Shader * _shader, std::vector<glm::vec2> _points);

	virtual void update(Step * _step) override;
};