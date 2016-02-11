#pragma once
#include <shader/ShaderComponent.h>

class ShaderComponentCircularMask : public ShaderComponent {
public:

	ShaderComponentCircularMask(ComponentShaderBase* _shader, float _ratio);

	void load() override;
	std::string getVertexVariablesString() override;
	std::string getVertexBodyString() override;
	std::string getFragmentBodyString() override;
	std::string getFragmentVariablesString() override;
	std::string getOutColorMod() override;
	void configureUniforms(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOption, NodeRenderable* _nodeRenderable) override;

	void setRatio(float _ratio);
	float getRatio() const;

private:
	float angle;
	float ratio;
	GLint angleLoc;
};