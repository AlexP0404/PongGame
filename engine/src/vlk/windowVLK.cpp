#include "windowVLK.hpp"

#include <cstdint>
#include <cstring> //memset
#include <iostream>
#include <stdexcept> //throw runtime_error
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef NDEBUG
const bool enabledValidationLayers = false;
#else
const bool enabledValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

windowVLK::windowVLK(int pWindowWidth, int pWindowHeight,
                     const std::string_view &pWindowTitle)
    : mWindowTitle(pWindowTitle) {
  mFrameBufferResized = false; // init variables
  initWindow(pWindowWidth, pWindowHeight);
  initVulkan();
}

void windowVLK::frameBufferResizeCallback(GLFWwindow *pWindow, int pWidth,
                                          int pHeight) {
  auto app = reinterpret_cast<windowVLK *>(glfwGetWindowUserPointer(pWindow));
  app->mFrameBufferResized = true;
}

windowVLK::~windowVLK() {
  cleanUpVulkan();
  glfwDestroyWindow(mWindowHandle);
  glfwTerminate();
}

void windowVLK::pollEvents() { glfwPollEvents(); }

bool windowVLK::windowShouldClose() {
  return glfwWindowShouldClose(mWindowHandle);
}

GLFWwindow *windowVLK::getWindowHandle() { return mWindowHandle; }

void windowVLK::initWindow(int pWindowWidth, int pWindowHeight) {
  glfwInit(); // init GLFW
  glfwWindowHint(GLFW_CLIENT_API,
                 GLFW_NO_API); // tell glfw using Vulkan not OpenGL
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // not resizable rn

  mWindowHandle = glfwCreateWindow(pWindowWidth, pWindowHeight,
                                   mWindowTitle.data(), nullptr,
                                   nullptr); // create window handle
  if (mWindowHandle == nullptr)
    throw std::runtime_error("Failed to open GLFW window!");
  glfwSetWindowUserPointer(mWindowHandle, this); // move the cursor to game
  glfwSetFramebufferSizeCallback(
      mWindowHandle,
      frameBufferResizeCallback); //(doesn't do anything rn because not
  // resizable) called when the window resizes
  // so the swapchain can be rebuilt
}

void windowVLK::initVulkan() {
  createInstance();
  setupDebugMessenger();
}

void windowVLK::createInstance() {
  if (enabledValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("Validation layers enabled, but not available!");
  }
  VkApplicationInfo appInfo{};
  zeroInitializeStruct(appInfo);
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = mWindowTitle.data();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Alex's Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  zeroInitializeStruct(createInfo);
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions = getRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  if (enabledValidationLayers) {
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    // allows the debug info for instance creation and destruction
    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
  }
  VkResult res = vkCreateInstance(&createInfo, nullptr, &mInstance);
  if (res == VK_ERROR_LAYER_NOT_PRESENT) {
    throw std::runtime_error("Validation layer(s) not found!");
  }
  if (res != VK_SUCCESS) {
    throw std::runtime_error("Failed to create instance!");
  }
}

bool windowVLK::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
  for (auto validationLayer : validationLayers) {
    for (const auto &layerProperties : availableLayers) {
      if (strcmp(validationLayer, layerProperties.layerName) == 0) {
        return true; // the validation layer was found
      }
    }
  }
  return false;
}

inline const std::vector<const char *> windowVLK::getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  // fix annoying validation layer issue
  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  extensions.push_back("VK_KHR_get_physical_device_properties2");

  if (enabledValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void windowVLK::cleanUpVulkan() {
  if (enabledValidationLayers) {
    DestroyDebugUtilsMessengerEXT(mInstance, debugMessenger, nullptr);
  }
  vkDestroyInstance(mInstance, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL windowVLK::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

void windowVLK::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {

  // TODO::figure out why some of these flags are causing validation errors!
  zeroInitializeStruct(createInfo);
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
}

void windowVLK::setupDebugMessenger() {
  if (!enabledValidationLayers)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  populateDebugMessengerCreateInfo(createInfo);

  if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr,
                                   &debugMessenger) != VK_SUCCESS)
    throw std::runtime_error("Failed to set up debug messenger!");
}

template <typename T> void windowVLK::zeroInitializeStruct(T &a) {
  static_assert(std::is_class<T>::value, "T must be a struct! (or class)");
  memset(&a, 0, sizeof(a));
}
