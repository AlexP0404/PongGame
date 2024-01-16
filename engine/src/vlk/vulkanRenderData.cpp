#include "vulkanRenderData.hpp"
#include <stdexcept>

VulkanRenderData::VulkanRenderData() {}

VulkanRenderData::~VulkanRenderData() {}

void VulkanRenderData::initQueue(const VkDevice &pDev, uint32_t pIndex,
                                 QueueTypes pQueType) {
  switch (pQueType) {
  case GRAPHICS:
    vkGetDeviceQueue(pDev, pIndex, 0, &mGraphicsQueue);
    break;
  case PRESENT:
    vkGetDeviceQueue(pDev, pIndex, 0, &mPresentQueue);
    break;
  default:
    throw std::runtime_error("BAD QUEUE TYPE GIVEN!");
  }
}
