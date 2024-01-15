#include "renderer.hpp"

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const glm::vec4 &pColor) {}

void Renderer::DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                        const textureVLK &pTexture, float pTilingFactor,
                        const glm::vec4 pTintColor) {}

void Renderer::renderScreen() {}
