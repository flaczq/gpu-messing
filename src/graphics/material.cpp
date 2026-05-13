#include "../configs/math_config.hpp"
#include "../configs/log_config.hpp"
#include "material.h"
#include "shader.h"
#include <map>
#include <memory>

Material::Material(const std::string& name, std::shared_ptr<Shader> shader)
	: m_name(name),
	  m_shader(std::move(shader))
{
}

void Material::addVec3Uniform(const std::string& name, glm::vec3 uniform) {
	m_vec3Uniforms.insert_or_assign(name, uniform);
}

void Material::addBoolUniform(const std::string& name, bool uniform) {
	m_boolUniforms.insert_or_assign(name, uniform);
}

void Material::apply() {
	for (const auto& [name, val] : m_vec3Uniforms) {
		m_shader->setVec3fv(name, val);
	}
	for (const auto& [name, val] : m_boolUniforms) {
		m_shader->setBool(name, val);
	}
	// TODO: other types
}