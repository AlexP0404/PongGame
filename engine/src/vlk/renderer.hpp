#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cstdint>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "circleVertex.hpp"
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
  void DrawCircle(const glm::vec2 &pPosition, const float pRadius,
                  const glm::vec3 &pColor, const uint32_t pCircleID);
  void DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                const glm::vec3 &pColor, const uint32_t pQuadID);

  void Flush();
  void BeginBatch();

private:
  std::shared_ptr<VulkanInit> mVLKInit;
  std::shared_ptr<WindowVLK> mWindowVLK;
  VulkanRenderData mVLKData;
  bool mFrameBufferResized;
  bool mNewCircleAdded;
  bool mNewQuadAdded;
  uint32_t mNumQuadsDrawn;
  uint32_t mNumCirclesDrawn;
  static constexpr glm::vec4 QUAD_VERTEX_POS[4] = {{-0.5f, -0.5f, 0.0f, 1.0f},
                                                   {0.5f, -0.5f, 0.0f, 1.0f},
                                                   {0.5f, 0.5f, 0.0f, 1.0f},
                                                   {-0.5f, 0.5f, 0.0f, 1.0f}};

  std::unordered_map<std::string, textureVLK> mTextures;

  static const glm::vec2 convertPosition(const glm::vec2 &pPosition);
  static const glm::vec2 convertSize(const glm::vec2 &pSize);
  static std::array<Vertex, 4> CreateQuad(const glm::vec2 &pPosition,
                                          const glm::vec2 &pSize,
                                          const glm::vec3 &pColor,
                                          const uint32_t pQuadID);
  static std::array<CircleVertex, 4> CreateCircle(const glm::vec2 &pPosition,
                                                  const float pRadius,
                                                  const glm::vec3 &pColor,
                                                  const uint32_t pCircleID);

  void EndBatch();
};

#endif // !RENDERER_HPP
