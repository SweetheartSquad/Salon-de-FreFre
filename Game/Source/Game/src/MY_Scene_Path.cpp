#pragma once

#include <MY_Scene_Path.h>
#include <Mouse.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <MeshInterface.h>
#include <RenderOptions.h>
#include <Keyboard.h>
#include <VerticalLinearLayout.h>
#include <MY_ResourceManager.h>

MY_Scene_Path::MY_Scene_Path(Game * _game):
	Scene(_game),
	pathActive(false),
	shader(new ComponentShaderBase(true)),
	uiLayer(0, 0, 0, 0)
{
	shader->addComponent(new ShaderComponentMVP(shader));
	shader->addComponent(new ShaderComponentTexture(shader));
	shader->compileShader();

	auto sd = sweet::getWindowDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);

	auto mainLayout = new VerticalLinearLayout(uiLayer.world);
	mainLayout->setRationalHeight(1.0f, &uiLayer);
	mainLayout->setRationalWidth(1.0f,  &uiLayer);
	mainLayout->horizontalAlignment = kCENTER;
	mainLayout->verticalAlignment   = kMIDDLE;
	uiLayer.addChild(mainLayout);

	auto direction = new NodeUI(uiLayer.world);
	direction->setWidth(50.f);
	direction->setHeight(50.f);
	direction->background->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("direction")->texture);
	mainLayout->addChild(direction);

	positionInd = new NodeUI(uiLayer.world);
	positionInd->setWidth(5);
	positionInd->setHeight(5);
	uiLayer.addChild(positionInd);

	MeshInterface * pathMesh = new MeshInterface(GL_LINES, GL_DYNAMIC_DRAW);
	path = new MeshEntity(pathMesh, shader);

	uiLayer.childTransform->addChild(path);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
}

void MY_Scene_Path::update(Step * _step){
	auto sd = sweet::getWindowDimensions();
	positionInd->firstParent()->translate(mouse->mouseX(), mouse->mouseY(), 0.f, false);
	if(keyboard->keyJustUp(GLFW_KEY_SPACE)) {
		pathActive = !pathActive;
	}
	if(pathActive){
		path->mesh->vertices.push_back(Vertex(mouse->mouseX(), mouse->mouseY(), 0.f, 1.f, 0.f, 0.f, 1.f));
		path->mesh->indices.push_back(path->mesh->vertices.size() - 1);
		if(path->mesh->vertices.size() > 1) {
			path->mesh->indices.push_back(path->mesh->vertices.size() - 1);
		}
		path->mesh->dirty = true;
		
		glm::vec2 ndc(
			mouse->mouseX()/sd.x - 0.5f,
			mouse->mouseY()/sd.y - 0.5f
		);

		float radius = glm::length(ndc);
		float angle = glm::degrees(glm::atan(ndc.y, ndc.x));

		points.push_back(glm::vec2(angle, radius));
	}
	if(keyboard->keyJustUp(GLFW_KEY_S) && keyboard->control) {
		exportJson();
	}
	Scene::update(_step);
	uiLayer.resize(0, sd.x, 0, sd.y);
	uiLayer.update(_step);
}

void MY_Scene_Path::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
}

void MY_Scene_Path::exportJson(){
	auto sd = sweet::getWindowDimensions();
	std::stringstream json;
	json << "{\n";
	json << "\t\"windowSize\": [" << sd.x << ", " << sd.y << "],\n";
	json << "\t\"points\" : [\n";
	int idx = 0;
	for(auto point : points) {
		if(idx == points.size() - 1) {
			json << "\t\t[" << point.x << "," << point.y << "]\n";		
		}else {
			json << "\t\t[" << point.x << "," << point.y << "],\n";	
		}
		++idx;
	}
	json << "\t]\n";
	json << "}";
	std::ofstream file("assets/path.json");
	file << json.str();
	file.close();
}
