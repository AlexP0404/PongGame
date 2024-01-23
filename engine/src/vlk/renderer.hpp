#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <iostream>
#include <memory>

#include "textureVLK.hpp"
#include "vertex.hpp"
#include "vulkanInit.hpp"
#include "vulkanRenderData.hpp"
#include "windowVLK.hpp"

struct GLFWwindow;

class Renderer {
public:
  // pass the windowVLK as a shared_ptr so the GLFWwindow doesnt destruct before
  // renderer
  Renderer(std::shared_ptr<WindowVLK> pWindow,
           const std::string_view &pWindowTitle);
  ~Renderer();
  Renderer(Renderer &r) { std::cerr << "WARNING::Copying the renderer!\n"; }
  void setFrameBufferResized(bool pFrameBufferResized = false);

  // Vulkan funcs
  void devWaitIdle();
  void renderScreen();

  // drawing funcs
  void DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                const glm::vec4 &pColor, const uint32_t pQuadID);

  void DrawQuad(const glm::vec3 &pPosition, const glm::vec2 &pSize,
                const glm::vec4 &pColor);

  void DrawQuad(const glm::mat4 &pTransform, const glm::vec4 &pColor);

  // texture drawing
  void DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                const textureVLK &pTexture, float pTilingFactor = 1.0f,
                const glm::vec4 pTintColor = glm::vec4(1.0f));

  void Flush();

private:
  std::shared_ptr<VulkanInit> mVLKInit;
  std::shared_ptr<WindowVLK> mWindowVLK;
  VulkanRenderData mVLKData;
  bool mFrameBufferResized;

  static const glm::vec2 convertPosition(const glm::vec2 &pPosition);
  static const glm::vec2 convertSize(const glm::vec2 &pSize);
  static std::array<Vertex, 4> CreateQuad(const glm::vec2 &pPosition,
                                          const glm::vec2 &pSize,
                                          const glm::vec4 &pColor,
                                          const uint32_t pQuadID);

  uint32_t mNumQuadsDrawn;
  const glm::vec4 QUAD_VERTEX_POS[4] = {{-0.5f, -0.5f, 0.0f, 1.0f},
                                        {0.5f, -0.5f, 0.0f, 1.0f},
                                        {0.5f, 0.5f, 0.0f, 1.0f},
                                        {-0.5f, 0.5f, 0.0f, 1.0f}};

  void BeginBatch();
  void EndBatch();
};

#endif // !RENDERER_HPP
