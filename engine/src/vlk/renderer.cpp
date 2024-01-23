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
  mNumQuadsDrawn = 0;
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
                                           const glm::vec3 &pColor,
                                           const uint32_t pQuadID) {
  Vertex v0;
  v0.pos = pPosition;
  v0.color = pColor;
  v0.entityID = pQuadID;

  Vertex v1;
  v1.pos = {pPosition.x + pSize.x, pPosition.y};
  v1.color = pColor;
  v1.entityID = pQuadID;

  Vertex v2;
  v2.pos = {pPosition.x + pSize.x, pPosition.y + pSize.y};
  v2.color = pColor;
  v2.entityID = pQuadID;

  Vertex v3;
  v3.pos = {pPosition.x, pPosition.y + pSize.y};
  v3.color = pColor;
  v3.entityID = pQuadID;

  return {v0, v1, v2, v3};
}

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const glm::vec3 &pColor, const uint32_t pQuadID) {
  /* DrawQuad({pPosition.x, pPosition.y, 0.0f}, pSize, pColor); */
  // if quad already exists, don't draw a new one, but update ubo position
  if (mVLKData.mVertices.at(pQuadID * 4).color == pColor) {
    mVLKData.updateEntityPos(pQuadID, convertPosition(pPosition));
    return;
  }

  if (mNumQuadsDrawn >= mVLKData.MAX_QUAD_COUNT) {
    Flush();
    BeginBatch();
  }
  auto quad = CreateQuad(convertPosition(pPosition), convertSize(pSize), pColor,
                         pQuadID);
  // quadID = current quads drawn

  mVLKData.mVertices.insert(mVLKData.mVertices.begin() + (pQuadID * 4),
                            quad.begin(), quad.end());
  mNumQuadsDrawn++;
  mVLKData.initNewEntity(); // rebuild swapchain with new vertex data
  // add quad vertices to the vertices vector
}

void Renderer::DrawQuad(const glm::vec3 &pPosition, const glm::vec2 &pSize,
                        const glm::vec3 &pColor) {
  glm::mat4 transform = glm::translate(glm::mat4(1.0f), pPosition) *
                        glm::scale(glm::mat4(1.0f), {pSize.x, pSize.y, 1.0f});
  DrawQuad(transform, pColor);
}

void Renderer::DrawQuad(const glm::mat4 &pTransform, const glm::vec3 &pColor) {
  constexpr size_t quadVertexCount = 4;
  for (size_t i = 0; i < quadVertexCount; i++) {
    Vertex v;
    v.pos = pTransform * QUAD_VERTEX_POS[i];
    v.color = pColor;
    mVLKData.mVertices.push_back(v);
    mNumQuadsDrawn++;
  }
}

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const textureVLK &pTexture, float pTilingFactor,
                        const glm::vec4 pTintColor) {}

void Renderer::renderScreen() {
  Flush();
  mVLKData.drawFrame(mFrameBufferResized); // should only need to resize once
  /* BeginBatch(); */
}

void Renderer::Flush() { mVLKData.drawIndexed(mNumQuadsDrawn); }

void Renderer::BeginBatch() {
  mVLKData.mVertices.clear();
  mNumQuadsDrawn = 0;
  mVLKData.mVertices.resize(mVLKData.MAX_VERTEX_COUNT);
}
