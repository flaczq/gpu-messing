#pragma once

#include "../configs/math_config.hpp"
#include <map>
#include <memory>

class Shader;

class Material {
public:
	Material(const std::string& name, std::shared_ptr<Shader> shader);

	const std::string& getName() const { return m_name; }
	Shader* getShader() { return m_shader.get(); }

	void addVec3Uniform(const std::string& name, glm::vec3 uniform);
	void addBoolUniform(const std::string& name, bool uniform);
	void apply();

private:
	std::string m_name{};
	std::shared_ptr<Shader> m_shader;
	std::map<std::string, glm::vec3> m_vec3Uniforms;
	std::map<std::string, bool> m_boolUniforms;
};