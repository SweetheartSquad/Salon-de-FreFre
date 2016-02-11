#pragma once

#include <MY_Scene_Path.h>
#include <Mouse.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <MeshInterface.h>
#include <RenderOptions.h>
#include <Keyboard.h>

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
		points.push_back(glm::vec2(mouse->mouseX(), mouse->mouseY()));
	}
	if(keyboard->keyJustUp(GLFW_KEY_S) && keyboard->control) {
		exportJson();
	}
	Scene::update(_step);
	auto sd = sweet::getWindowDimensions();
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
	json << "{";
	json << "\t\"windowSize\": [" << sd.x << ", " << sd.y << "],\n";
	json << "\t\"points\" : [\n";
	for(auto point : points) {
		json << "\t\t[" << point.x << "," << point.y << "],\n";
	}
	json << "\t]\n";
	json << "}";
	std::ofstream file("assets/path.json");
	file << json.str();
	file.close();
}
