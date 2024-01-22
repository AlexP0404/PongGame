#include "renderer.hpp"
#include "vertex.hpp"

#include <glm/gtc/matrix_transform.hpp>
/* #include <glm/gtc/type_ptr.hpp> */

static glm::vec2 sWinDimensions;

Renderer::Renderer(std::shared_ptr<WindowVLK> pWindow,
                   const std::string_view &pWindowTitle)
    : mWindowVLK(pWindow) {
  mVLKInit = std::shared_ptr<VulkanInit>(new VulkanInit);
  mVLKInit->Init(pWindow->getWindowHandle(), pWindowTitle);
  sWinDimensions = {mVLKInit->mSwapChainExtent.width,
                    mVLKInit->mSwapChainExtent.height};

  mVLKData.initRenderData(mVLKInit);
  mFrameBufferResized = false;
}
// this may not work if the device is already destroyed. it shouldnt tho?
Renderer::~Renderer() {}

void Renderer::setFrameBufferResized(bool pFrameBufferResized) {
  mFrameBufferResized = pFrameBufferResized;
}

void Renderer::devWaitIdle() { mVLKData.devWaitIdle(); }

const glm::vec2 Renderer::convertPosition(const glm::vec2 &pPosition) {
  // center around top left instead of window center

  return pPosition / sWinDimensions * 2.0f - glm::vec2(1.0f);
  /* return ret / sWinDimensions; // put in range 0-1 */
}

const glm::vec2 Renderer::convertSize(const glm::vec2 &pSize) {
  return pSize / sWinDimensions * 2.0f;
}

std::array<Vertex, 4> Renderer::CreateQuad(const glm::vec2 &pPosition,
                                           const glm::vec2 &pSize,
                                           const glm::vec4 &pColor) {
  Vertex v0;
  v0.pos = pPosition;
  v0.color = pColor;

  Vertex v1;
  v1.pos = {pPosition.x + pSize.x, pPosition.y};
  v1.color = pColor;

  Vertex v2;
  v2.pos = {pPosition.x + pSize.x, pPosition.y + pSize.y};
  v2.color = pColor;

  Vertex v3;
  v3.pos = {pPosition.x, pPosition.y + pSize.y};
  v3.color = pColor;

  return {v0, v1, v2, v3};
}

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const glm::vec4 &pColor) {
  /* DrawQuad({pPosition.x, pPosition.y, 0.0f}, pSize, pColor); */
  auto quad =
      CreateQuad(convertPosition(pPosition), convertSize(pSize), pColor);
  mVertices.insert(mVertices.end(), quad.begin(), quad.end());
  // add quad vertices to the vertices vector
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
  Flush();
  mVLKData.drawFrame(mFrameBufferResized); // should only need to resize once
}

void Renderer::Flush() {
  mVLKData.drawIndexed(mVertices);
  mVertices.clear();
}

void Renderer::BeginBatch() { mVertices.clear(); }
