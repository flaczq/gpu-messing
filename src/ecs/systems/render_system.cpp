#include "render_system.h"
#include "../../ecs/components.h"

void RenderSystem::update(entt::registry registry, float screenWidth, float screenHeight) {
	auto view = registry.view<TransformComponent, MeshComponent, MaterialComponent>();

	for (auto entity : view) {
		auto& tranform = view.get<TransformComponent>(entity);
		auto& mesh = view.get<MeshComponent>(entity);
		auto& material = view.get<MaterialComponent>(entity);

		// set state (shaders, textures)
		glUseProgram(material.shaderID);
		glBindTexture(GL_TEXTURE_2D, material.textureID);
		
		// matrices
		glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), tranform.position);
		//TODO rotation+scale

		// send to GPU
		glUniformMatrix4fv(glGetUniformLocation(material.shaderID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));

		// render pure VAO
		glBindVertexArray(mesh.vaoID);
		glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
	}
}