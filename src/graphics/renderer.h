#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include "../ecs/components/render_component.hpp"
#include "../ecs/systems/physics_system.h"
#include <vector>

class Camera;
class Model;
class Material;

struct RendererLight {
	glm::vec3 direction;
	glm::vec3 color;
};
struct RendererCommand {
	Model* model;
	Material* material;
	//glm::mat4 projection;
	//glm::mat4 view;
	//glm::vec3 viewPos;
	glm::mat4 modelMatrix;
	glm::mat3 normalMatrix;
	glm::vec3 position;
};
struct RendererImmediateCommand {
	unsigned int VAO;
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	glm::vec3 size;
	glm::vec3 center;
	glm::vec3 color;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class Renderer {
public:
	// Meyer's Singleton
	static Renderer& getInstance();
	Renderer(const Renderer&) = delete;
	void operator=(const Renderer&) = delete;

	bool init(GLFWwindow* window, PhysicsSystem& physicsSystem, Camera* camera);
	void toggleRenderMode();
	void toggleRenderDebugMode();
	void beginFrame(unsigned int screenWidth, unsigned int screenHeight);
	void beginFrameMinimap(unsigned int minimapWidth, unsigned int minimapHeight);
	void flush();
	void renderImmediate();
	void renderFrameBufferTexture();
	void endFrameMinimap();
	void endFrame();

	void setCamera(Camera* camera) { m_camera = camera; }
	//void setStencilReqd(bool stencilReqd) { m_stencilReqd = stencilReqd; }
	//void setBlendingReqd(bool blendingReqd) { m_blendingReqd = blendingReqd; }
	RendererLight* getRendererLight() { return &m_light; }
	void setLightDir(glm::vec3 lightDir) { m_light.direction = lightDir; }

private:
	// hidden constructor
	Renderer();

	RendererRenderMode m_renderMode = RendererRenderMode::STANDARD;
	RendererRenderDebugMode m_renderDebugMode = RendererRenderDebugMode::NONE;
	RendererLight m_light{};
	glm::mat4 activeProjection{};
	//bool m_stencilReqd = false;
	//bool m_blendingReqd = false;

	void sortQueueByMaterial(std::vector<RendererCommand>& queue) const;
	void sortQueueByDistance(std::vector<RendererCommand>& queue) const;
	void renderSortedQueue(std::vector<RendererCommand>& queue, const std::string& name) const;
};