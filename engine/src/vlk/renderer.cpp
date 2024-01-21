#include "renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
/* #include <glm/gtc/type_ptr.hpp> */

Renderer::Renderer(std::shared_ptr<WindowVLK> pWindow,
                   const std::string_view &pWindowTitle)
    : mWindowVLK(pWindow) {
  mVLKInit = std::shared_ptr<VulkanInit>(new VulkanInit);
  mVLKInit->Init(pWindow->getWindowHandle(), pWindowTitle);
  mVLKData.initRenderData(mVLKInit);
  mFrameBufferResized = false;
}
// this may not work if the device is already destroyed. it shouldnt tho?
Renderer::~Renderer() {}

void Renderer::setFrameBufferResized(bool pFrameBufferResized) {
  mFrameBufferResized = pFrameBufferResized;
}

void Renderer::devWaitIdle() { mVLKData.devWaitIdle(); }

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const glm::vec4 &pColor) {
  DrawQuad({pPosition.x, pPosition.y, 0.0f}, pSize, pColor);
}

void Renderer::DrawQuad(const glm::vec3 &pPosition, const glm::vec2 &pSize,
                        const glm::vec4 &pColor) {
  glm::mat4 transform = glm::translate(glm::mat4(1.0f), pPosition) *
                        glm::scale(glm::mat4(1.0f), {pSize.x, pSize.y, 1.0f});
  DrawQuad(transform, pColor);
}

void Renderer::DrawQuad(const glm::mat4 &pTransform, const glm::vec4 &pColor) {
  constexpr size_t quadVertexCount = 4;
  for (size_t i = 0; i < quadVertexCount; i++) {
    Vertex v;
    v.pos = pTransform * QUAD_VERTEX_POS[i];
    v.color = pColor;
    mVertices.push_back(v);
  }
}

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const textureVLK &pTexture, float pTilingFactor,
                        const glm::vec4 pTintColor) {}

void Renderer::renderScreen() {
  /* Flush(); */
  mVLKData.drawFrame(mFrameBufferResized); // should only need to resize once
}

void Renderer::Flush() {
  mVLKData.drawIndexed(mVertices);
  mVertices.clear();
}
