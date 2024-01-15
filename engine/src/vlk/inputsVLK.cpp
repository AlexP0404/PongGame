#include "inputsVLK.hpp"

#include "engineVLK.hpp"

inputVLK::~inputVLK() { windowHandle = nullptr; }

void inputVLK::updateKeyStates() {
  windowHandle = EngineVLK::Get().PFN_GetWindowVLK()->getWindowHandle();
}

bool inputVLK::IsKeyDown(KeyCode key) {
  if (windowHandle ==
      nullptr) // somehow if window isn't create yet or already destroyed
    return false;

  int state = glfwGetKey(windowHandle, (int)key);
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}
