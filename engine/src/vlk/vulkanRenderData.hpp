#ifndef VULKAN_RENDER_DATA_HPP
#define VULKAN_RENDER_DATA_HPP

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

enum QueueTypes {
  GRAPHICS,
  PRESENT,
  COMPUTE, // idk if this is one lmao
  UNKNOWN
};

class VulkanRenderData {
public:
  explicit VulkanRenderData();
  ~VulkanRenderData();

  void initQueue(const VkDevice &pDev, uint32_t pIndex, QueueTypes pQueType);

  VkQueue mGraphicsQueue;
  VkQueue mPresentQueue;

  std::vector<VkImage> mSwapChainImages;
  std::vector<VkImageView> mSwapChainImageViews;
  std::vector<VkFramebuffer> mFramebuffers;

private:
};

#endif
