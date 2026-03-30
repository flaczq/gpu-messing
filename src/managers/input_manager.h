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

    void processMouseMovement(double xpos, double ypos);
    void processMouseScroll(double yoffset);
    void setKeyState(int key, bool pressed);
    bool isKeyPressed(int key) const;
    void reset();

    float getOffsetX() const { return m_offsetX; }
    float getOffsetY() const { return m_offsetY; }
    float getScrollOffset() const { return m_scrollOffset; }

private:
	// hidden constructor
    InputManager();

    // last key index
    bool m_keys[GLFW_KEY_LAST] = { false };
    bool m_firstMouse = true;
    float m_lastX{}, m_lastY{};
    float m_offsetX{}, m_offsetY{};
    float m_scrollOffset{};
};