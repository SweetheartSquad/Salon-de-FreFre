#pragma once

#include <MY_Scene_Base.h>

class StereoCamera;

class MY_Scene_Main : public MY_Scene_Base{
public:
	StereoCamera * vrCam;

	MY_Scene_Main(Game * _game);

	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
};