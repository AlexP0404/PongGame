#include "inputsVLK.hpp"

#include "engineVLK.hpp"

inputVLK::~inputVLK() { windowHandle = nullptr; }

void inputVLK::updateKeyStates() {
  windowHandle = EngineVLK::Get().getWindowHandle();
}

bool inputVLK::IsKeyDown(KeyCode key) {
  int state = glfwGetKey(windowHandle, (int)key);
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}
