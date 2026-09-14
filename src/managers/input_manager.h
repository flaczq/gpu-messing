#pragma once

#include "../configs/gl_config.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class InputManager {
public:
    // Meyer's Singleton
    static InputManager& getInstance();
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;

    bool init();
    void copyKeys();
    void processMouseMovement(double xPosition, double yPosition);
    void processMouseScroll(double yOffset);
    void setKeyState(int key, bool pressed);
    bool isKeyDown(int key) const;
    bool isKeyPressed(int key) const;
    void reset();

    float getOffsetX() const { return m_offsetX; }
    float getOffsetY() const { return m_offsetY; }
    float getScrollOffset() const { return m_scrollOffset; }

private:
	// hidden constructor
    InputManager();

    // last key index
    bool m_currKeys[GLFW_KEY_LAST] = { false };
    bool m_prevKeys[GLFW_KEY_LAST] = { false };
    bool m_firstMouse = true;
    // default values
    float m_lastX = 640.0f;
    float m_lastY = 384.0f;
    float m_offsetX = 0.0f;
    float m_offsetY = 0.0f;
    float m_scrollOffset = 0.0f;
};