#include "engineVLK.hpp"

// allows things like the input class to access the current application instance
// while avoiding creating a whole new engine object
static EngineVLK *s_Instance = nullptr;
EngineVLK &EngineVLK::Get() { return *s_Instance; }

EngineVLK::EngineVLK() {
  s_Instance = this; // set the current application instance
}

EngineVLK::~EngineVLK() {
  s_Instance = nullptr; // instance is no longer valid
}

GLFWwindow *EngineVLK::getWindowHandle() { return m_window; }
