#include "renderer.hpp"

Renderer::Renderer() {
  mWindow = nullptr;
  mFrameBufferResized = false;
}

Renderer::~Renderer() {
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}

bool Renderer::initWindow(int pWindowWidth, int pWindowHeight,
                          const std::string &windowTitle) {
  glfwInit(); // init GLFW
  glfwWindowHint(GLFW_CLIENT_API,
                 GLFW_NO_API); // tell glfw using Vulkan not OpenGL
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // not resizable rn

  mWindow = glfwCreateWindow(pWindowWidth, pWindowHeight, windowTitle.c_str(),
                             nullptr,
                             nullptr); // create window handle
  if (mWindow == nullptr)
    return false;
  glfwSetWindowUserPointer(mWindow, this); // move the cursor to game
  glfwSetFramebufferSizeCallback(
      mWindow,
      frameBufferResizeCallback); //(doesn't do anything rn because not
                                  // resizable) called when the window resizes
                                  // so the swapchain can be rebuilt
  return true;
}

void Renderer::frameBufferResizeCallback(GLFWwindow *pWindow, int pWidth,
                                         int pHeight) {
  auto app = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(pWindow));
  app->mFrameBufferResized = true;
}

void Renderer::pollWindowEvents() { glfwPollEvents(); }

bool Renderer::windowShouldClose() { return glfwWindowShouldClose(mWindow); }

GLFWwindow *Renderer::getWindow() { return mWindow; }

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const glm::vec4 &pColor) {}

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const textureVLK &pTexture, float pTilingFactor,
                        const glm::vec4 pTintColor) {}

void Renderer::renderScreen() { pollWindowEvents(); }
