#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glm/glm.hpp>
#include <iostream>

#include "textureVLK.hpp"

class Renderer {
public:
  Renderer();
  ~Renderer();
  Renderer(Renderer &r) { std::cerr << "WARNING::Copying the renderer!\n"; }

  // Vulkan funcs/objs

  // drawing funcs
  static void DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                       const glm::vec4 &pColor);

  static void DrawQuad(const glm::vec2 &pPosition, const glm::vec2 &pSize,
                       const textureVLK &pTexture, float pTilingFactor = 1.0f,
                       const glm::vec4 pTintColor = glm::vec4(1.0f));

  void renderScreen();

private:
};

#endif // !RENDERER_HPP
