#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <iostream>
#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "textureVLK.hpp"

class Renderer {
public:
  Renderer();
  ~Renderer();
  Renderer(Renderer &r) {
    std::cerr << "WARNING::Copying the renderer!\n";
    mWindow = r.mWindow;
  }

  // GLFW funcs
  bool initWindow(int pWindowWidth, int pWindowHeight,
                  const std::string &pWindowTitle);
  static void frameBufferResizeCallback(GLFWwindow *pWindow, int pWidth,
                                        int pHeight);
  void pollWindowEvents();
  bool windowShouldClose();
  GLFWwindow *getWindow();

  // Vulkan funcs/objs

  // drawing funcs
  static void DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                       const glm::vec4 &pColor);

  static void DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                       const textureVLK &pTexture, float pTilingFactor = 1.0f,
                       const glm::vec4 pTintColor = glm::vec4(1.0f));

  void renderScreen();

  bool mFrameBufferResized;

private:
  GLFWwindow *mWindow;
};

#endif // !RENDERER_HPP
