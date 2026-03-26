#pragma once

#include "../utils/commongl.h"

class Shader {
public:
	unsigned int ID;

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

private:
	unsigned int vertex, fragment;
};