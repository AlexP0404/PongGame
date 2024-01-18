#include "renderer.hpp"

Renderer::Renderer(GLFWwindow *pWindowHandle,
                   const std::string_view &pWindowTitle)
/* : mWindowHandle(pWindowHandle), mWindowTitle(pWindowTitle) { */
{
  mVLKInit = std::shared_ptr<VulkanInit>(new VulkanInit);
  mVLKInit->Init(pWindowHandle, pWindowTitle);
  mVLKData.initRenderData(mVLKInit);
  mFrameBufferResized = false;
}
// this may not work if the device is already destroyed. it shouldnt tho?
Renderer::~Renderer() {}

void Renderer::setFrameBufferResized(bool pFrameBufferResized) {
  mFrameBufferResized = pFrameBufferResized;
}

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const glm::vec4 &pColor) {}

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const textureVLK &pTexture, float pTilingFactor,
                        const glm::vec4 pTintColor) {}

void Renderer::renderScreen() {
  mVLKData.drawFrame(mFrameBufferResized); // should only need to resize once
}
