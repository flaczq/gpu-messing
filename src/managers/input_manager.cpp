#include "input_manager.h"
#include <algorithm>

InputManager& InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

InputManager::InputManager()
    : m_lastX(640.0f),
      m_lastY(384.0f),
      m_offsetX(0.0f),
      m_offsetY(0.0f),
      m_scrollOffset(0.0f)
{
    // just to be sure
    std::fill(std::begin(m_currKeys), std::end(m_currKeys), false);
    std::fill(std::begin(m_prevKeys), std::end(m_prevKeys), false);
}

void InputManager::update() {
    memcpy(m_prevKeys, m_currKeys, sizeof(m_currKeys));
}

void InputManager::processMouseMovement(double xpos, double ypos) {
    if (m_firstMouse) {
        m_lastX = (float)xpos;
        m_lastY = (float)ypos;
        m_firstMouse = false;
    }

    m_offsetX = (float)xpos - m_lastX;
    m_offsetY = m_lastY - (float)ypos;

    m_lastX = (float)xpos;
    m_lastY = (float)ypos;
}

void InputManager::processMouseScroll(double scrollOffset) {
    m_scrollOffset = static_cast<float>(scrollOffset);
}

void InputManager::setKeyState(int key, bool pressed) {
    m_currKeys[key] = pressed;
}

bool InputManager::isKeyDown(int key) const {
    // continuous click
    return m_currKeys[key];
}

bool InputManager::isKeyPressed(int key) const {
    // single click
    return m_currKeys[key] && !m_prevKeys[key];
}

void InputManager::reset() {
    m_offsetX = 0.0f;
    m_offsetY = 0.0f;
    m_scrollOffset = 0.0f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // other keys safe
    if (key < 0) {
        return;
    }

    if (action == GLFW_PRESS) {
        InputManager::getInstance().setKeyState(key, true);
    } else if (action == GLFW_RELEASE) {
        InputManager::getInstance().setKeyState(key, false);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    InputManager::getInstance().processMouseMovement(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    InputManager::getInstance().processMouseScroll(yoffset);
}