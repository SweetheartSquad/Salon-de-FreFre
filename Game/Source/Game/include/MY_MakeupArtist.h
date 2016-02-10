#pragma once

#include <MeshEntity.h>

class MY_MakeupArtist : public MeshEntity{
public:
	MY_MakeupArtist(Shader * _shader);

	virtual void update(Step * _step) override;
};