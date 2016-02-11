#pragma once

#include <MeshEntity.h>

class MY_MakeupArtist : public MeshEntity{
public:
	std::vector<glm::vec2> points;
	int currentPointIdx;

	MY_MakeupArtist(Shader * _shader, std::vector<glm::vec2> _points);

	virtual void update(Step * _step) override;
};