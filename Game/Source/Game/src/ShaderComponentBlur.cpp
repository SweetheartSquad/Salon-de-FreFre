#pragma once

#include <ShaderComponentBlur.h>
#include <shader/ShaderVariables.h>
#include <shader/ComponentShaderBase.h>

ShaderComponentBlur::ShaderComponentBlur(ComponentShaderBase* _shader) : 
	ShaderComponent(_shader),
	blurAmount(0.9f),
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
			res << "modFrag = mix(modFrag, texture(" << GL_UNIFORM_ID_TEXTURE_SAMPLER << "[0], " << GL_IN_OUT_FRAG_UV << " + vec2(" << x / 100.f << "," << y / 100.f << ")  ), blurAmount)" << SEMI_ENDL;
		}
	}
	res << "modFrag.rgb *= blurAmount" << SEMI_ENDL;
	//return "if(atan(fragUV.y - 0.5, fragUV.x - 0.5) > circMaskAngle){discard;}" + ENDL;
	return res.str();
}

std::string ShaderComponentBlur::getFragmentVariablesString() {
	std::stringstream res;
	//http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
	res << "float rand(vec2 co){"
		"return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);"
		"}" << SEMI_ENDL;
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
