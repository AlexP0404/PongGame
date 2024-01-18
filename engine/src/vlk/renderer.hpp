#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glm/glm.hpp>
#include <iostream>
#include <memory>

#include "textureVLK.hpp"
#include "vulkanInit.hpp"
#include "vulkanRenderData.hpp"

struct GLFWwindow;

class Renderer {
public:
  Renderer(GLFWwindow *pWindowHandle, const std::string_view &pWindowTitle);
  ~Renderer();
  Renderer(Renderer &r) { std::cerr << "WARNING::Copying the renderer!\n"; }
  void setFrameBufferResized(bool pFrameBufferResized = false);

  // Vulkan funcs/objs

  // drawing funcs
  static void DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                       const glm::vec4 &pColor);

  static void DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                       const textureVLK &pTexture, float pTilingFactor = 1.0f,
                       const glm::vec4 pTintColor = glm::vec4(1.0f));

  void renderScreen();

private:
  std::shared_ptr<VulkanInit> mVLKInit;
  VulkanRenderData mVLKData;
  bool mFrameBufferResized;
};

#endif // !RENDERER_HPP
