#include "soldier_scene.h"

SoldierScene::SoldierScene() {
    soldierModel = nullptr;
    modelShader = nullptr;
    camera = nullptr;
}

SoldierScene::~SoldierScene() {
    soldierModel.reset();
    modelShader.reset();
    camera = nullptr;
}

void SoldierScene::init(GLFWwindow* window) {
    soldierModel = std::make_unique<Model>("../assets/models/Soldier.glb");
    modelShader = std::make_unique<Shader>("../shaders/model.vert", "../shaders/model.frag");

    Camera camera{ window };
    camera.init(window);
}

void SoldierScene::update(GLFWwindow* window, float dt) {
    camera.processInput(window, dt);

    // transformation matrix: clip = projectionM * viewM * modelM * local
    // 1. local * modelM            -> world
    // 2. world * viewM             -> space (lookAt())
    // 3. space * projectionM       -> clip
    // 4. clip  * viewportTransform -> screen
    
    // model
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(3.0f, 0.0f, 3.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, dt * glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(100.0f));

    // projection
    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.getFov()), camera.getAspect(), 1.0f, 100.0f);

    // view
    view = glm::mat4(1.0f);
    view = camera.getViewMatrix();
}

void SoldierScene::render() {
    modelShader->use();

    // scale fix and optimization with CPU computation
    modelShader->setMat4fv("projection", projection);
    modelShader->setMat4fv("view", view);
    modelShader->setMat4fv("model", model);
    modelShader->setMat3fv("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

    modelShader->setVec3fv("lightDir", glm::vec3(0.5f, -1.0f, -0.5f));
    modelShader->setVec3fv("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    modelShader->setVec3fv("viewPos", camera.getPosition());

    soldierModel->draw(*modelShader);
}

void SoldierScene::end() {
    //audio.stop()
}

SceneID SoldierScene::getSceneID() {
    return sceneID;
}

Camera& SoldierScene::getCamera() {
    return camera;
}