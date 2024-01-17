#ifndef VULKAN_RENDER_DATA_HPP
#define VULKAN_RENDER_DATA_HPP

#include "vulkanInit.hpp"

#include <memory>
#include <vector>

class VulkanRenderData {
public:
  explicit VulkanRenderData();
  ~VulkanRenderData();
  void initRenderData(std::shared_ptr<VulkanInit> pInit);

public:
  VkQueue mGraphicsQueue;
  VkQueue mPresentQueue;

  std::vector<VkImage> mSwapChainImages;
  std::vector<VkImageView> mSwapChainImageViews;
  std::vector<VkFramebuffer> mFramebuffers;

  VkRenderPass mRenderPass;
  VkPipelineLayout mPipelineLayout;
  VkPipeline mGraphicsPipeline;

private:
  void initQueues();
  void getSwapChainImages();
  void createImageViews();
  void createRenderPass();
  void createGraphicsPipeline();
  void createFramebuffers();

  static std::vector<char> readShader(const std::string &pFilename);
  VkShaderModule createShaderModule(const std::vector<char> &code);

private:
  std::shared_ptr<VulkanInit> mInit;
};

#endif
