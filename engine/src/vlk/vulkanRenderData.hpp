#ifndef VULKAN_RENDER_DATA_HPP
#define VULKAN_RENDER_DATA_HPP

#include "vulkanInit.hpp"

#include <cstdint>
#include <memory>
#include <vector>

class VulkanRenderData {
public:
  explicit VulkanRenderData();
  ~VulkanRenderData();
  void initRenderData(std::shared_ptr<VulkanInit> pInit);
  void drawFrame(bool pFrameBufferResized);
  // these are used by the renderer class
  static void BeginBatch();
  static void EndBatch();
  static void Flush();

public:
  VkQueue mGraphicsQueue;
  VkQueue mPresentQueue;

  std::vector<VkImage> mSwapChainImages;
  std::vector<VkImageView> mSwapChainImageViews;
  std::vector<VkFramebuffer> mFramebuffers;

  VkRenderPass mRenderPass;
  VkPipelineLayout mPipelineLayout;
  VkPipeline mGraphicsPipeline;

  VkCommandPool mCommandPool;
  std::vector<VkCommandBuffer> mCommandBuffers;

  std::vector<VkSemaphore> mImgAvailSemaphores;
  std::vector<VkSemaphore> mRndrFinSemaphores;
  std::vector<VkFence> mInFlightFences;

private:
  void initQueues();
  void getSwapChainImages();
  void createImageViews();
  void recreateSwapchain();
  void createRenderPass();
  void createGraphicsPipeline();
  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void recordCommandBuffer(VkCommandBuffer pCommandBuffer,
                           uint32_t pImageIndex);
  void createSyncObjects();

  static std::vector<char> readShader(const std::string &pFilename);
  VkShaderModule createShaderModule(const std::vector<char> &code);

private:
  std::shared_ptr<VulkanInit> mInit;
  uint32_t mCurrentFrame;
};

#endif
