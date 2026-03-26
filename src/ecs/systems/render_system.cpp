#include "render_system.h"
#include "../../ecs/components.h"

void RenderSystem::update(entt::registry registry, float screenWidth, float screenHeight) {
	// camera
	auto cameraView = registry.view<CameraComponent, TransformComponent>();
	auto cameraEntity = cameraView.front();
	auto& camera = cameraView.get<CameraComponent>(cameraEntity);
	auto& cameraTransform = cameraView.get<TransformComponent>(cameraEntity);

	glm::mat4 view = glm::lookAt(cameraTransform.position, cameraTransform.position + camera.front, camera.up);
	glm::mat4 projection = glm::perspective(glm::radians(camera.fov), screenWidth / screenHeight, camera.nearPlane, camera.farPlane);

	// render all
	auto renderComponents = registry.view<TransformComponent, MeshComponent, MaterialComponent>();

	for (auto renderEntity : renderComponents) {
		auto& transform = renderComponents.get<TransformComponent>(renderEntity);
		auto& mesh = renderComponents.get<MeshComponent>(renderEntity);
		auto& material = renderComponents.get<MaterialComponent>(renderEntity);

		// set state (shaders, textures)
		glUseProgram(material.shaderID);
		glBindTexture(GL_TEXTURE_2D, material.textureID);
		
		// matrix
		glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);
		model = glm::rotate(model, transform.rotiation.x, { 1.0f, 1.0f, 1.0f });

		// send to GPU
		glUniformMatrix4fv(glGetUniformLocation(material.shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view)); //&view[0][0]
		glUniformMatrix4fv(glGetUniformLocation(material.shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection)); //&projection[0][0]
		glUniformMatrix4fv(glGetUniformLocation(material.shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model)); //&model[0][0]

		// render VAO
		glBindVertexArray(mesh.vaoID);
		glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
	}
}