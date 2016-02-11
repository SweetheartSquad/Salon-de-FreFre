#pragma once

#include <ShaderComponentCircularMask.h>
#include <shader/ShaderVariables.h>
#include <shader/ComponentShaderBase.h>

ShaderComponentCircularMask::ShaderComponentCircularMask(ComponentShaderBase* _shader, float _ratio) : 
	ShaderComponent(_shader),
	angle(0.f),
	ratio(_ratio),
	angleLoc(-1)
{
}

void ShaderComponentCircularMask::load() {
	angleLoc = glGetUniformLocation(shader->getProgramId(), "circMaskAngle");
	ShaderComponent::load();
}

std::string ShaderComponentCircularMask::getVertexVariablesString() {
	return "";
}

std::string ShaderComponentCircularMask::getVertexBodyString() {
	return "";
}

std::string ShaderComponentCircularMask::getFragmentBodyString() {
	return "if(atan(fragUV.y - 0.5, fragUV.x - 0.5) > circMaskAngle){discard;}" + ENDL;
}

std::string ShaderComponentCircularMask::getFragmentVariablesString() {
	return "uniform float circMaskAngle" + SEMI_ENDL;
}

std::string ShaderComponentCircularMask::getOutColorMod() {
	return "";
}

void ShaderComponentCircularMask::setRatio(float _ratio) {
	ratio = _ratio;	
	angle = _ratio * 360.f;
	angle = fmod(angle, 360.f);
    if (angle < 0.f){
        angle += 360.f;
	}
	angle -= 180.f;
	angle = glm::radians(angle);
	makeDirty();
}

float ShaderComponentCircularMask::getRatio() const {
	return ratio;
}

void ShaderComponentCircularMask::configureUniforms(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOption, NodeRenderable* _nodeRenderable) {
	if(shader->isDirty()){
		glUniform1f(angleLoc, angle);
	}
}
