#pragma once
#include <Scene.h>
#include <UILayer.h>

class MY_Scene_Path : public Scene {
public:
	
	bool pathActive;

	ComponentShaderBase * shader;
	std::vector<glm::vec2> points;
	UILayer uiLayer;
	NodeUI * positionInd;

	MeshEntity * path;

	explicit MY_Scene_Path(Game* _game);

	void update(Step * _step) override;
	void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	void exportJson();
};
