#ifndef VULKAN_INIT_HPP
#define VULKAN_INIT_HPP

#include <cstdint>
#include <cstring> //memset
#include <optional>
#include <string_view>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct GLFWwindow;

class VulkanInit {
public: // funcs
  VulkanInit() {}
  ~VulkanInit();
  void Init(GLFWwindow *pWindowHandle, const std::string_view &pWindowTitle);

  template <typename T> inline static void zeroInitializeStruct(T &a) {
    static_assert(std::is_class<T>::value, "T must be a struct! (or class)");
    memset(&a, 0, sizeof(a));
  }

public: // objects
  VkInstance mInstance;
  VkSurfaceKHR mSurface;
  VkDevice mLogicalDevice;
  VkSwapchainKHR mSwapChain;
  VkFormat mSwapChainImageFormat;
  VkExtent2D mSwapChainExtent;

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

  // swap chain and window surface
  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice pDevice);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  void createSwapChain();

private: // objects
  GLFWwindow *mWindowHandle;
  std::string_view mWindowTitle;

  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  VkDebugUtilsMessengerEXT debugMessenger;
};
#endif
