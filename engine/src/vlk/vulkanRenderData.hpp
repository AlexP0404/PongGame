#ifndef VULKAN_RENDER_DATA_HPP
#define VULKAN_RENDER_DATA_HPP

#include "vertex.hpp"
#include "vulkanInit.hpp"

#include <glm/glm.hpp>

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

  void drawIndexed(uint32_t pNumVerticesToDraw = 0);

  const int MAX_FRAMES_IN_FLIGHT = 2;
  const int MAX_QUAD_COUNT = 100;
  const int MAX_VERTEX_COUNT = MAX_QUAD_COUNT * 4;
  const int MAX_INDEX_COUNT = MAX_QUAD_COUNT * 6;
  std::vector<Vertex> mVertices; // this will be the verticies to be graphed
private:
  VkQueue mGraphicsQueue;
  VkQueue mPresentQueue;

  std::vector<VkImage> mSwapChainImages;
  std::vector<VkImageView> mSwapChainImageViews;
  std::vector<VkFramebuffer> mFramebuffers;

  VkRenderPass mRenderPass;

  VkDescriptorSetLayout mDescSetLayout;

  VkPipelineLayout mPipelineLayout;
  VkPipeline mQuadGraphicsPipeline;

  VkCommandPool mCommandPool;
  std::vector<VkCommandBuffer> mCommandBuffers;

  VkBuffer mVertexBuffer;
  VkDeviceMemory mVertexBufferMem;
  VkBuffer mIndexBuffer;
  VkDeviceMemory mIndexBuferMem;
  std::vector<uint16_t> mIndices;
  std::vector<VkBuffer> mUniformBuffers;
  std::vector<VkDeviceMemory> mUniformBuffersMemory;
  std::vector<void *> mUniformBuffersMapped;

  std::vector<VkSemaphore> mImgAvailSemaphores;
  std::vector<VkSemaphore> mRndrFinSemaphores;
  std::vector<VkFence> mInFlightFences;

private:
  void initQueues();
  void getSwapChainImages();
  void createImageViews();
  void recreateSwapchain();
  void createRenderPass();
  void createDescriptorSetLayout();
  void createGraphicsPipeline();
  void createFramebuffers();
  void createCommandPool();
  void createBuffer(VkDeviceSize pSize, VkBufferUsageFlags pUsage,
                    VkMemoryPropertyFlags pProperties, VkBuffer &pBuffer,
                    VkDeviceMemory &pBufferMem);
  void copyBuffer(VkBuffer pSrcBuffer, VkBuffer pDstBuffer, VkDeviceSize pSize);
  void createVertexBuffer();
  void createIndexBuffer();
  void createUniformBuffers();
  void updateUniformBuffer(uint32_t pCurrentImage);
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
