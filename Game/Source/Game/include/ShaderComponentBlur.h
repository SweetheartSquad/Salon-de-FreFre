#pragma once
#include <shader/ShaderComponent.h>

class ShaderComponentBlur : public ShaderComponent {
public:

	ShaderComponentBlur(ComponentShaderBase* _shader);

	void load() override;
	std::string getVertexVariablesString() override;
	std::string getVertexBodyString() override;
	std::string getFragmentBodyString() override;
	std::string getFragmentVariablesString() override;
	std::string getOutColorMod() override;
	void configureUniforms(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOption, NodeRenderable* _nodeRenderable) override;

	float blurAmount;
private:
	GLint blurLoc;
};