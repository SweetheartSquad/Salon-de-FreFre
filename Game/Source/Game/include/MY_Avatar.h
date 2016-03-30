#pragma once

#include <MeshEntity.h>
//#include <Animation.h>

class Camera;

class MY_Avatar : public Entity{
public:

	MeshEntity
		* base,
		* head,
		* eyes,
		* cheeks,
		* lips,
		* lashes,
		* hat;

	Camera * playerCam;

	MY_Avatar(Shader * _shader, Camera * _playerCam);
	~MY_Avatar();

	virtual void update(Step * _step) override;
};