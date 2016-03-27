#pragma once

#include <MeshEntity.h>
#include <Animation.h>

class MY_MakeupArtist : public Entity{
public:
	std::vector<glm::vec2> points;
	int currentPointIdx;

	bool paused;
	
	float breathe, blink;
	Animation<float> * breatheAnim, * blinkAnim;

	MeshEntity
		* torso,
		* head,
		* handR,
		* handL,
		* footR,
		* footL,
		* eyeR,
		* eyeL;

	// polar coordinates
	float angle, radius;

	MY_MakeupArtist(Shader * _shader, std::vector<glm::vec2> _points);
	~MY_MakeupArtist();

	virtual void update(Step * _step) override;
};