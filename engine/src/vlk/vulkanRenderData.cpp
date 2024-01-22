#include "vulkanRenderData.hpp"

#include "ubo.hpp"
#include "utils.hpp"

#include <cassert>
#include <cstdint>
#include <fstream>
#include <vector>

VulkanRenderData::VulkanRenderData() {
  mCurrentFrame = 0;
  mNumIndiciesToDraw = 0;
}

VulkanRenderData::~VulkanRenderData() {
  assert(mInit);
  //
  //
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroyBuffer(mInit->mLogicalDevice, mUniformBuffers[i], nullptr);
    vkFreeMemory(mInit->mLogicalDevice, mUniformBuffersMemory[i], nullptr);
  }
  vkDestroyDescriptorSetLayout(mInit->mLogicalDevice, mDescSetLayout, nullptr);
  vkDestroyBuffer(mInit->mLogicalDevice, mIndexBuffer, nullptr);
  vkFreeMemory(mInit->mLogicalDevice, mIndexBuferMem, nullptr);
  vkDestroyBuffer(mInit->mLogicalDevice, mVertexBuffer, nullptr);
  vkFreeMemory(mInit->mLogicalDevice, mVertexBufferMem, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(mInit->mLogicalDevice, mImgAvailSemaphores[i], nullptr);
    vkDestroySemaphore(mInit->mLogicalDevice, mRndrFinSemaphores[i], nullptr);
    vkDestroyFence(mInit->mLogicalDevice, mInFlightFences[i], nullptr);
  }
  vkDestroyCommandPool(mInit->mLogicalDevice, mCommandPool, nullptr);

  for (auto framebuffer : mFramebuffers) {
    vkDestroyFramebuffer(mInit->mLogicalDevice, framebuffer, nullptr);
  }
  vkDestroyPipeline(mInit->mLogicalDevice, mQuadGraphicsPipeline, nullptr);
  vkDestroyPipelineLayout(mInit->mLogicalDevice, mPipelineLayout, nullptr);
  vkDestroyRenderPass(mInit->mLogicalDevice, mRenderPass, nullptr);
  for (auto imageView : mSwapChainImageViews)
    vkDestroyImageView(mInit->mLogicalDevice, imageView, nullptr);
}

void VulkanRenderData::initRenderData(std::shared_ptr<VulkanInit> pInit) {
  mInit = pInit;
  initQueues();
  getSwapChainImages();
  createImageViews();
  createRenderPass();
  createDescriptorSetLayout();
  createGraphicsPipeline();
  createFramebuffers();
  createCommandPool();
  createVertexBuffer();
  createIndexBuffer();
  createUniformBuffers();
  createCommandBuffers();
  createSyncObjects();
}

void VulkanRenderData::devWaitIdle() {
  vkDeviceWaitIdle(mInit->mLogicalDevice);
}

void VulkanRenderData::initQueues() {
  assert(mInit); // make sure vulkan init has completed and been set
  vkGetDeviceQueue(mInit->mLogicalDevice,
                   mInit->mQFIndices.graphicsFamily.value(), 0,
                   &mGraphicsQueue);
  vkGetDeviceQueue(mInit->mLogicalDevice,
                   mInit->mQFIndices.graphicsFamily.value(), 0, &mPresentQueue);
}

void VulkanRenderData::getSwapChainImages() {
  assert(mInit);
  uint32_t imageCount;
  vkGetSwapchainImagesKHR(mInit->mLogicalDevice, mInit->mSwapChain, &imageCount,
                          nullptr);
  mSwapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(mInit->mLogicalDevice, mInit->mSwapChain, &imageCount,
                          mSwapChainImages.data());
}

void VulkanRenderData::createImageViews() {
  assert(mInit);
  mSwapChainImageViews.resize(mSwapChainImages.size());

  for (size_t i = 0; i < mSwapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    Utils::zeroInitializeStruct(createInfo);
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = mSwapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = mInit->mSwapChainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(mInit->mLogicalDevice, &createInfo, nullptr,
                          &mSwapChainImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create image views!");
    }
  }
}

void VulkanRenderData::createRenderPass() {
  VkAttachmentDescription colorAttachment{};
  Utils::zeroInitializeStruct(colorAttachment);
  colorAttachment.format = mInit->mSwapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  Utils::zeroInitializeStruct(colorAttachmentRef);
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  Utils::zeroInitializeStruct(subpass);
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  // THIS IS (LAYOUT = 0) FROM THE FRAGMENT SHADER CODE
  // change below to read in more data from fragment shader
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo{};
  Utils::zeroInitializeStruct(renderPassInfo);
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  if (vkCreateRenderPass(mInit->mLogicalDevice, &renderPassInfo, nullptr,
                         &mRenderPass) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create render pass!");
  }
}

void VulkanRenderData::createDescriptorSetLayout() {
  assert(mInit);
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  Utils::zeroInitializeStruct(uboLayoutBinding);
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  Utils::zeroInitializeStruct(layoutInfo);
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &uboLayoutBinding;

  VK_CHECK(vkCreateDescriptorSetLayout(mInit->mLogicalDevice, &layoutInfo,
                                       nullptr, &mDescSetLayout));
}

void VulkanRenderData::createGraphicsPipeline() {
  assert(mInit);
  auto vertShaderCode = readShader("shaders/vert.spv");
  auto fragShaderCode = readShader("shaders/frag.spv");

  VkShaderModule vShaderModule = createShaderModule(vertShaderCode);
  VkShaderModule fShaderModule = createShaderModule(fragShaderCode);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  Utils::zeroInitializeStruct(vertShaderStageInfo);
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  // sending shader to vertex stage of pipeline
  vertShaderStageInfo.module = vShaderModule; // set module
  vertShaderStageInfo.pName = "main";         // tell what function to read 1st

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  Utils::zeroInitializeStruct(fragShaderStageInfo);
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  Utils::zeroInitializeStruct(vertexInputInfo);
  auto bindingDesc = Vertex::getBindingDescription();
  auto attrDesc = Vertex::getAttributeDescriptions();

  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attrDesc.size());
  vertexInputInfo.pVertexAttributeDescriptions = attrDesc.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  Utils::zeroInitializeStruct(inputAssembly);
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  Utils::zeroInitializeStruct(viewport);
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)mInit->mSwapChainExtent.width;
  viewport.height = (float)mInit->mSwapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  Utils::zeroInitializeStruct(scissor);
  scissor.offset = {0, 0};
  scissor.extent = mInit->mSwapChainExtent;

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  Utils::zeroInitializeStruct(dynamicState);
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineViewportStateCreateInfo viewportState{};
  Utils::zeroInitializeStruct(viewportState);
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  Utils::zeroInitializeStruct(rasterizer);
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  Utils::zeroInitializeStruct(multisampling);
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  Utils::zeroInitializeStruct(colorBlendAttachment);
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  Utils::zeroInitializeStruct(colorBlending);
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  Utils::zeroInitializeStruct(pipelineLayoutInfo);
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &mDescSetLayout;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(mInit->mLogicalDevice, &pipelineLayoutInfo,
                             nullptr, &mPipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  Utils::zeroInitializeStruct(pipelineInfo);
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = nullptr;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = mPipelineLayout;
  pipelineInfo.renderPass = mRenderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(mInit->mLogicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &mQuadGraphicsPipeline) != VK_SUCCESS)
    throw std::runtime_error("Failed to create graphics pipeline!");

  vkDestroyShaderModule(mInit->mLogicalDevice, vShaderModule, nullptr);
  vkDestroyShaderModule(mInit->mLogicalDevice, fShaderModule, nullptr);
}

std::vector<char> VulkanRenderData::readShader(const std::string &pFilename) {
  std::ifstream file(pFilename, std::ios::ate | std::ios::binary);
  // open back of file and read in binary format

  if (!file.is_open()) { // has to be valid file
    throw std::runtime_error("Failed to open file! Given: " + pFilename);
  }
  // end position gives file size in bytes
  size_t fileSize = (size_t)file.tellg();
  // allocate buffer of that size
  std::vector<char> buffer(fileSize);
  // go back to beginning
  file.seekg(0);
  // read in bytes to buffer
  file.read(buffer.data(), fileSize);
  // close file
  file.close();

  return buffer;
}

VkShaderModule
VulkanRenderData::createShaderModule(const std::vector<char> &pCode) {
  assert(mInit);
  VkShaderModuleCreateInfo createInfo{};
  Utils::zeroInitializeStruct(createInfo);
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = pCode.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(pCode.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(mInit->mLogicalDevice, &createInfo, nullptr,
                           &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create shader module!");
  }
  return shaderModule;
}

void VulkanRenderData::createFramebuffers() {
  assert(mInit);
  mFramebuffers.resize(mSwapChainImageViews.size());
  for (size_t i = 0; i < mSwapChainImageViews.size(); i++) {
    VkImageView attachments[] = {mSwapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    Utils::zeroInitializeStruct(framebufferInfo);
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = mRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = mInit->mSwapChainExtent.width;
    framebufferInfo.height = mInit->mSwapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(mInit->mLogicalDevice, &framebufferInfo, nullptr,
                            &mFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create framebuffer!");
    }
  }
}

void VulkanRenderData::createCommandPool() {
  assert(mInit);
  VkCommandPoolCreateInfo poolInfo{};
  Utils::zeroInitializeStruct(poolInfo);
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = mInit->mQFIndices.graphicsFamily.value();

  if (vkCreateCommandPool(mInit->mLogicalDevice, &poolInfo, nullptr,
                          &mCommandPool) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create command pool!");
  }
}

void VulkanRenderData::createBuffer(VkDeviceSize pSize,
                                    VkBufferUsageFlags pUsage,
                                    VkMemoryPropertyFlags pProperties,
                                    VkBuffer &pBuffer,
                                    VkDeviceMemory &pBufferMem) {
  assert(mInit);
  VkBufferCreateInfo bufferInfo{};
  Utils::zeroInitializeStruct(bufferInfo);
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = pSize;
  bufferInfo.usage = pUsage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(mInit->mLogicalDevice, &bufferInfo, nullptr, &pBuffer) !=
      VK_SUCCESS)
    throw std::runtime_error("Failed to create vertex buffer!");

  VkMemoryRequirements memRequ;
  vkGetBufferMemoryRequirements(mInit->mLogicalDevice, pBuffer, &memRequ);

  VkMemoryAllocateInfo allocInfo{};
  Utils::zeroInitializeStruct(allocInfo);
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequ.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memRequ.memoryTypeBits, pProperties);

  if (vkAllocateMemory(mInit->mLogicalDevice, &allocInfo, nullptr,
                       &pBufferMem) != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate vertex buffer memory!");

  vkBindBufferMemory(mInit->mLogicalDevice, pBuffer, pBufferMem, 0);
}

void VulkanRenderData::copyBuffer(VkBuffer pSrcBuffer, VkBuffer pDstBuffer,
                                  VkDeviceSize pSize) {
  assert(mInit);
  VkCommandBufferAllocateInfo allocInfo{};
  Utils::zeroInitializeStruct(allocInfo);
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = mCommandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(mInit->mLogicalDevice, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  Utils::zeroInitializeStruct(beginInfo);
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  VkBufferCopy copyRegion{};
  Utils::zeroInitializeStruct(copyRegion);
  copyRegion.size = pSize;
  vkCmdCopyBuffer(commandBuffer, pSrcBuffer, pDstBuffer, 1, &copyRegion);

  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  Utils::zeroInitializeStruct(submitInfo);
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(mGraphicsQueue);

  vkFreeCommandBuffers(mInit->mLogicalDevice, mCommandPool, 1, &commandBuffer);
}

void VulkanRenderData::createVertexBuffer() {
  mVertices.resize(MAX_VERTEX_COUNT);
  VkDeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBuffMem;

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBuffMem);

  void *data;
  void *vertexData;
  vkMapMemory(mInit->mLogicalDevice, stagingBuffMem, 0, bufferSize, 0, &data);
  memcpy(data, mVertices.data(), (size_t)bufferSize);
  vkUnmapMemory(mInit->mLogicalDevice, stagingBuffMem);

  createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMem);

  copyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

  vkDestroyBuffer(mInit->mLogicalDevice, stagingBuffer, nullptr);
  vkFreeMemory(mInit->mLogicalDevice, stagingBuffMem, nullptr);
}

uint32_t VulkanRenderData::findMemoryType(uint32_t pTypeFilter,
                                          VkMemoryPropertyFlags pProperties) {
  assert(mInit);
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(mInit->mPhysicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if (pTypeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags &
                                   pProperties) == pProperties) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type!");
}

void VulkanRenderData::createIndexBuffer() {
  // fill index array (may be wasteful idk)
  mIndices.resize(MAX_INDEX_COUNT);
  uint32_t offset = 0;
  for (size_t i = 0; i < MAX_INDEX_COUNT; i += 6) {
    mIndices[i + 0] = 0 + offset;
    mIndices[i + 1] = 1 + offset;
    mIndices[i + 2] = 2 + offset;
    mIndices[i + 3] = 2 + offset;
    mIndices[i + 4] = 3 + offset;
    mIndices[i + 5] = 0 + offset;

    offset += 4;
  }

  assert(mInit);
  VkDeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBuffMem;

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBuffMem);

  void *data;
  vkMapMemory(mInit->mLogicalDevice, stagingBuffMem, 0, bufferSize, 0, &data);
  memcpy(data, mIndices.data(), (size_t)bufferSize);
  vkUnmapMemory(mInit->mLogicalDevice, stagingBuffMem);

  createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBuferMem);

  copyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

  vkDestroyBuffer(mInit->mLogicalDevice, stagingBuffer, nullptr);
  vkFreeMemory(mInit->mLogicalDevice, stagingBuffMem, nullptr);
}

void VulkanRenderData::createUniformBuffers() {
  assert(mInit);
  VkDeviceSize bufferSize = sizeof(UBO);

  mUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  mUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
  mUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 mUniformBuffers[i], mUniformBuffersMemory[i]);
    vkMapMemory(mInit->mLogicalDevice, mUniformBuffersMemory[i], 0, bufferSize,
                0, &mUniformBuffersMapped[i]);
  }
}

void VulkanRenderData::createCommandBuffers() {
  mCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  VkCommandBufferAllocateInfo allocInfo{};
  Utils::zeroInitializeStruct(allocInfo);
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = mCommandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

  if (vkAllocateCommandBuffers(mInit->mLogicalDevice, &allocInfo,
                               mCommandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create command buffer!");
  }
}

void VulkanRenderData::recordCommandBuffer(VkCommandBuffer pCommandBuffer,
                                           uint32_t pImageIndex) {
  assert(mInit);
  VkCommandBufferBeginInfo beginInfo{};
  Utils::zeroInitializeStruct(beginInfo);
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(pCommandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  Utils::zeroInitializeStruct(renderPassInfo);
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = mRenderPass;
  renderPassInfo.framebuffer = mFramebuffers[pImageIndex];
  renderPassInfo.renderArea.offset.x = 0;
  renderPassInfo.renderArea.offset.y = 0;
  renderPassInfo.renderArea.extent = mInit->mSwapChainExtent;
  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(pCommandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    mQuadGraphicsPipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(mInit->mSwapChainExtent.width);
  viewport.height = static_cast<float>(mInit->mSwapChainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(pCommandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent = mInit->mSwapChainExtent;
  vkCmdSetScissor(pCommandBuffer, 0, 1, &scissor);

  VkBuffer vertexBuffers[] = {mVertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, vertexBuffers, offsets);

  vkCmdBindIndexBuffer(pCommandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

  /* vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, */
  /*                         pipelineLayout, 0, 1,
   * &descriptorSets[currentFrame], */
  /*                         0, nullptr); */

  vkCmdDrawIndexed(pCommandBuffer, static_cast<uint32_t>(mIndices.size()), 1, 0,
                   0, 0);

  vkCmdEndRenderPass(pCommandBuffer);

  if (vkEndCommandBuffer(pCommandBuffer) != VK_SUCCESS)
    throw std::runtime_error("Failed to record command buffer!");
}

void VulkanRenderData::createSyncObjects() {
  assert(mInit);
  mImgAvailSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  mRndrFinSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  Utils::zeroInitializeStruct(semaphoreInfo);
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  Utils::zeroInitializeStruct(fenceInfo);
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(mInit->mLogicalDevice, &semaphoreInfo, nullptr,
                          &mImgAvailSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(mInit->mLogicalDevice, &semaphoreInfo, nullptr,
                          &mRndrFinSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(mInit->mLogicalDevice, &fenceInfo, nullptr,
                      &mInFlightFences[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create semaphores!");
    }
  }
}

void VulkanRenderData::drawFrame(bool pFrameBufferResized) {
  assert(mInit);
  vkWaitForFences(mInit->mLogicalDevice, 1, &mInFlightFences[mCurrentFrame],
                  VK_TRUE, UINT64_MAX);
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      mInit->mLogicalDevice, mInit->mSwapChain, UINT64_MAX,
      mImgAvailSemaphores[mCurrentFrame], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapchain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  vkResetFences(mInit->mLogicalDevice, 1, &mInFlightFences[mCurrentFrame]);
  vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], 0);

  updateUniformBuffer(mCurrentFrame);
  recordCommandBuffer(mCommandBuffers[mCurrentFrame], imageIndex);

  VkSubmitInfo submitInfo{};
  Utils::zeroInitializeStruct(submitInfo);
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {mImgAvailSemaphores[mCurrentFrame]};
  VkPipelineStageFlags waitStage[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStage;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];

  VkSemaphore signalSemaphores[] = {mRndrFinSemaphores[mCurrentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo,
                    mInFlightFences[mCurrentFrame]) != VK_SUCCESS)
    throw std::runtime_error("Failed to submit draw command buffer!");

  VkPresentInfoKHR presentInfo{};
  Utils::zeroInitializeStruct(presentInfo);
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {mInit->mSwapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      pFrameBufferResized) {
    recreateSwapchain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swap chain image!");
  }

  mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT; // either 0 or 1
}

void VulkanRenderData::updateUniformBuffer(uint32_t pCurrentImage) {
  // you can figure out which quad is chosen using the % operator
  //
}

void VulkanRenderData::updateBuffers() {
  vkDeviceWaitIdle(mInit->mLogicalDevice); // this is terrible

  vkFreeCommandBuffers(mInit->mLogicalDevice, mCommandPool,
                       mCommandBuffers.size(), mCommandBuffers.data());
  vkDestroyBuffer(mInit->mLogicalDevice, mIndexBuffer, nullptr);
  vkFreeMemory(mInit->mLogicalDevice, mIndexBuferMem, nullptr);
  vkDestroyBuffer(mInit->mLogicalDevice, mVertexBuffer, nullptr);
  vkFreeMemory(mInit->mLogicalDevice, mVertexBufferMem, nullptr);

  createVertexBuffer();
  createIndexBuffer();
  createCommandBuffers();
}

void VulkanRenderData::drawIndexed(uint32_t pNumVerticesToDraw /*=0*/) {
  mNumIndiciesToDraw = pNumVerticesToDraw / 4 * 6; // 6 indicies per quad

  updateBuffers();
}

void VulkanRenderData::recreateSwapchain() {
  // NOTE:This may cause issues because the swapchain is being destroyed and
  // recreated before these are being destroyed at all
  //
  mInit->updateWindowDimensions();
  for (auto framebuffer : mFramebuffers) {
    vkDestroyFramebuffer(mInit->mLogicalDevice, framebuffer, nullptr);
  }

  for (auto imageView : mSwapChainImageViews) {
    vkDestroyImageView(mInit->mLogicalDevice, imageView, nullptr);
  }

  createImageViews();
  createFramebuffers();
}
// TODO:: design the command buffer implementation so that the renderer is able
// to batch together commands
