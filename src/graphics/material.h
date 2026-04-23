#pragma once

#include "../configs/math_config.hpp"
#include <map>
#include <memory>

class Shader;

class Material {
public:
	Material(std::shared_ptr<Shader> shader);

	Shader* getShader() { return m_shader.get(); }
	void addVec3Uniform(const std::string& name, glm::vec3 vec3Uniform);
	
	void apply();

	bool isZDepthRequired() const { return m_zDepthRequired; }
	void setZDepthRequired(bool zDepthRequired) { m_zDepthRequired = zDepthRequired; }
	bool isStencilRequired() const { return m_stencilRequired; }
	void setStencilRequired(bool stencilRequired) { m_stencilRequired = stencilRequired; }

private:
	std::shared_ptr<Shader> m_shader;
	std::map<std::string, glm::vec3> m_vec3Uniforms;

	bool m_zDepthRequired = true;
	bool m_stencilRequired = false;
};