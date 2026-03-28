#include "soldier_scene.h"

SoldierScene::SoldierScene(Camera* camera) {
    this->camera = camera;

    soldier = std::make_unique<Model>("../assets/models/Soldier.glb");
    soldierShader = std::make_unique<Shader>("../shaders/model.vert", "../shaders/model.frag");
    lightShader = std::make_unique<Shader>("../shaders/light.vert", "../shaders/light.frag");
}

void SoldierScene::init() {
    std::vector<Vertex> lightVertices = calculateLightVertices();
    std::vector<unsigned int> lightIndices = calculateLightIndices();
    std::vector<Texture> lightTextures;
    lightMarker = std::make_unique<Mesh>(lightVertices, lightIndices, lightTextures);
}

void SoldierScene::update(float dt) {
    // SOLDIER
    static float time = 0.0f;
    time += dt;
    float x = sin(time);
    float z = cos(time);
    lightDir = glm::normalize(glm::vec3(x, -1.0f, z));

    rotation += rotationSpeed * dt;
    if (rotation >= 360.0f) {
        rotation -= 360.0f;
    }

    // transformation matrix: clip = projectionM * viewM * modelM * local
    // 1. local * modelM            -> world
    // 2. world * viewM             -> space (lookAt())
    // 3. space * projectionM       -> clip
    // 4. clip  * viewportTransform -> screen
    soldierModel = glm::mat4(1.0f);
    soldierModel = glm::translate(soldierModel, soldierPos);
    soldierModel = glm::rotate(soldierModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //soldierModel = glm::rotate(soldierModel, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    soldierModel = glm::scale(soldierModel, glm::vec3(100.0f));

    // projection
    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera->fov), camera->aspect, 0.1f, 100.0f);

    // view
    view = glm::mat4(1.0f);
    view = camera->getViewMatrix();

    // additional
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(soldierModel)));

    // LIGHT
    glm::vec3 lightMarkerPos = soldierPos - (glm::normalize(lightDir) * 5.0f);
    //lightMarkerPos.y += 2.0f;
    lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightMarkerPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));
}

void SoldierScene::render() {
    // SOLDIER
    soldierShader->use();

    // scale fix and optimization with CPU computation
    soldierShader->setMat4fv("projection", projection);
    soldierShader->setMat4fv("view", view);
    soldierShader->setMat4fv("model", soldierModel);
    soldierShader->setMat3fv("normalMatrix", normalMatrix);

    soldierShader->setVec3fv("lightDir", lightDir);
    soldierShader->setVec3fv("lightColor", glm::vec3(1.0f));
    soldierShader->setVec3fv("viewPos", camera->position);

    soldier->draw(*soldierShader);

    // LIGHT
    lightShader->use();

    lightShader->setMat4fv("projection", projection);
    lightShader->setMat4fv("view", view);
    lightShader->setMat4fv("model", lightModel);

    lightMarker->draw(*lightShader);
}

void SoldierScene::end() {
    //audio.stop()
}

std::vector<Vertex> SoldierScene::calculateLightVertices() {
    std::vector<Vertex> vertices;
    vertices.reserve(24);

    struct RawData { glm::vec3 pos; glm::vec3 norm; };

    RawData raw[] = {
        // back (normal: 0, 0, -1)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        // front (normal: 0, 0, 1)
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}}, {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}}, {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}},
        // left (normal: -1, 0, 0)
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}},
        // right (normal: 1, 0, 0)
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}}, {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
        // down (normal: 0, -1, 0)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}}, {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}}, {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}},
        // up (normal: 0, 1, 0)
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}
    };

    for (int i = 0; i < 24; ++i) {
        Vertex vertex{};
        vertex.Position = raw[i].pos;
        vertex.Normal = raw[i].norm;

        vertices.push_back(vertex);
    }

    return vertices;
}

std::vector<unsigned int> SoldierScene::calculateLightIndices() {
    return {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };
}