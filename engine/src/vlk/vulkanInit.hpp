#ifndef VULKAN_INIT_HPP
#define VULKAN_INIT_HPP

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include <vulkan/vulkan.h>

struct GLFWwindow;

class VulkanInit {
public: // funcs
  VulkanInit() {}
  ~VulkanInit();
  void Init(GLFWwindow *pWindowHandle, const std::string_view &pWindowTitle);

public: // objects
  VkInstance mInstance;
  VkSurfaceKHR mSurface;
  VkDevice mLogicalDevice;
  VkSwapchainKHR mSwapChain;

  struct QueueFamilyIndicies {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };
  QueueFamilyIndicies mQFIndices;

private: // funcs
  // instance funcs
  void createInstance();
  inline const std::vector<const char *> getRequiredExtensions();
  inline bool checkValidationLayerSupport();
  inline const void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);
  void setupDebugMessenger();
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);
  /* static void frameBufferResizeCallback(GLFWwindow *pWindow, int pWidth, */
  /* int pHeight); */

  // window surface
  void createSurface();

  // physical device
  void pickPhysicalDevice();
  VkPhysicalDevice
  pickBestPhysicalDevice(const std::vector<VkPhysicalDevice> &pDevices);
  QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice pDevice);

  // logical device
  void createLogicalDevice();

  template <typename T> inline void zeroInitializeStruct(T &a);

private: // objects
  GLFWwindow *mWindowHandle;
  std::string_view mWindowTitle;

  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  VkDebugUtilsMessengerEXT debugMessenger;
};
#endif
