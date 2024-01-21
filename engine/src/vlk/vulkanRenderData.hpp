#ifndef VULKAN_RENDER_DATA_HPP
#define VULKAN_RENDER_DATA_HPP

#include "vertex.hpp"
#include "vulkanInit.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <vector>

class VulkanRenderData {
public:
  explicit VulkanRenderData();
  ~VulkanRenderData();
  void initRenderData(std::shared_ptr<VulkanInit> pInit);
  void drawFrame(bool pFrameBufferResized);
  void devWaitIdle();
  // these are used by the renderer class

  void drawIndexed(const std::vector<Vertex> &pVerticies);

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

  VkBuffer mVertexBuffer;
  VkDeviceMemory mVertexBufferMem;
  VkBuffer mIndexBuffer;
  VkDeviceMemory mIndexBuferMem;
  std::vector<Vertex> mVerticies; // this will be the verticies to be graphed
  std::vector<uint16_t> mIndices;

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
  void createBuffer(VkDeviceSize pSize, VkBufferUsageFlags pUsage,
                    VkMemoryPropertyFlags pProperties, VkBuffer &pBuffer,
                    VkDeviceMemory &pBufferMem);
  void copyBuffer(VkBuffer pSrcBuffer, VkBuffer pDstBuffer, VkDeviceSize pSize);
  void createVertexBuffer();
  void createIndexBuffer();
  uint32_t findMemoryType(uint32_t pTypeFilter,
                          VkMemoryPropertyFlags pProperties);
  void createCommandBuffers();
  void recordCommandBuffer(VkCommandBuffer pCommandBuffer,
                           uint32_t pImageIndex);
  void updateBuffers();
  void createSyncObjects();

  static std::vector<char> readShader(const std::string &pFilename);
  VkShaderModule createShaderModule(const std::vector<char> &pCode);

private:
  std::shared_ptr<VulkanInit> mInit;
  uint32_t mCurrentFrame;
  uint32_t mNumIndiciesToDraw;
};

#endif
