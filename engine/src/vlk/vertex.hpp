#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <array>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDesc{};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(Vertex);
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDesc;
  }

  static std::array<VkVertexInputAttributeDescription, 2>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDesc{};
    attributeDesc[0].binding = 0;
    attributeDesc[0].location = 0;
    attributeDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT; // vec3
    attributeDesc[0].offset = offsetof(Vertex, pos);

    attributeDesc[1].binding = 0;
    attributeDesc[1].location = 1;
    attributeDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDesc[1].offset = offsetof(Vertex, color);

    return attributeDesc;
  }
};

#endif
