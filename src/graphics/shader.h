#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include <string>

class Shader {
public:
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Shader();

	void use() const;
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3fv(const std::string& name, const glm::vec3& value) const;
	void setVec4fv(const std::string& name, const glm::vec4& value) const;
	void setMat3fv(const std::string& name, const glm::mat3& value) const;
	void setMat4fv(const std::string& name, const glm::mat4& value) const;

	unsigned int getID() const;
private:
	unsigned int m_ID{};
	unsigned int m_vertex{}, m_fragment{};
};