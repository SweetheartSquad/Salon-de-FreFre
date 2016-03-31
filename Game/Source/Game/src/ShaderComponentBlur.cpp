#pragma once

#include <ShaderComponentBlur.h>
#include <shader/ShaderVariables.h>
#include <shader/ComponentShaderBase.h>

ShaderComponentBlur::ShaderComponentBlur(ComponentShaderBase* _shader) : 
	ShaderComponent(_shader),
	blurAmount(1.f),
	blurLoc(-1)
{
}

void ShaderComponentBlur::load() {
	blurLoc = glGetUniformLocation(shader->getProgramId(), "blurAmount");
	ShaderComponent::load();
}

std::string ShaderComponentBlur::getVertexVariablesString() {
	return "";
}

std::string ShaderComponentBlur::getVertexBodyString() {
	return "";
}

std::string ShaderComponentBlur::getFragmentBodyString() {
	std::stringstream res;
	for (signed long int x = -5; x <= 5; ++x){
		for (signed long int y = -5; y <= 5; ++y){
			res << "modFrag = mix(modFrag, texture(" << GL_UNIFORM_ID_TEXTURE_SAMPLER << "[0], " << GL_IN_OUT_FRAG_UV << " + vec2(" << x / 50.f << "," << y / 200.f << ")  ), blurAmount)" << SEMI_ENDL;
		}
	}
	res << "modFrag.rgb = mix(modFrag.rgb, vec3(0.8), 1.f-blurAmount)" << SEMI_ENDL;
	return res.str();
}

std::string ShaderComponentBlur::getFragmentVariablesString() {
	std::stringstream res;
	res << "uniform float blurAmount" + SEMI_ENDL;
	return res.str();
}

std::string ShaderComponentBlur::getOutColorMod() {
	return "";
}

void ShaderComponentBlur::configureUniforms(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOption, NodeRenderable* _nodeRenderable) {
	if (shader->isDirty()){
		glUniform1f(blurLoc, blurAmount);
	}
}
