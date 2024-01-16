#include "vulkanInit.hpp"
#include <cstdint>
#include <cstring> //memset
#include <iostream>
#include <set>
#include <stdexcept>

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

template <typename T> void VulkanInit::zeroInitializeStruct(T &a) {
  static_assert(std::is_class<T>::value, "T must be a struct! (or class)");
  memset(&a, 0, sizeof(a));
}

/* VulkanInit::VulkanInit() {} */

VulkanInit::~VulkanInit() {
  if (enabledValidationLayers) {
    DestroyDebugUtilsMessengerEXT(mInstance, debugMessenger, nullptr);
  }
  vkDestroyDevice(mLogicalDevice, nullptr);
  vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  vkDestroyInstance(mInstance, nullptr);
}

void VulkanInit::Init(GLFWwindow *pWindowHandle,
                      const std::string_view &pWindowTitle) {
  mWindowHandle = pWindowHandle;
  mWindowTitle = pWindowTitle;
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
}

void VulkanInit::createInstance() {
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

bool VulkanInit::checkValidationLayerSupport() {
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

inline const std::vector<const char *> VulkanInit::getRequiredExtensions() {
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

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInit::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

const void VulkanInit::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {

  zeroInitializeStruct(createInfo);
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
}

void VulkanInit::setupDebugMessenger() {
  if (!enabledValidationLayers)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  populateDebugMessengerCreateInfo(createInfo);

  if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr,
                                   &debugMessenger) != VK_SUCCESS)
    throw std::runtime_error("Failed to set up debug messenger!");
}

void VulkanInit::createSurface() {
  if (glfwCreateWindowSurface(mInstance, mWindowHandle, nullptr, &mSurface) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface!");
  }
}

void VulkanInit::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

  if (deviceCount == 0)
    throw std::runtime_error("Failed to find GPUs with Vulkan support!");
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

  mPhysicalDevice = pickBestPhysicalDevice(devices);

  if (mPhysicalDevice == VK_NULL_HANDLE)
    throw std::runtime_error("Failed to find suitable GPU!");
}

VkPhysicalDevice VulkanInit::pickBestPhysicalDevice(
    const std::vector<VkPhysicalDevice> &pDevices) {
  // just looks for descrete gpu with some specifc features
  VkPhysicalDevice potentialPhysDevice = VK_NULL_HANDLE;
  for (auto dev : pDevices) {
    VkPhysicalDeviceProperties devProperties;
    VkPhysicalDeviceFeatures devFeatures;
    vkGetPhysicalDeviceProperties(dev, &devProperties);
    vkGetPhysicalDeviceFeatures(dev, &devFeatures);

    QueueFamilyIndicies indicies = findQueueFamilies(dev);

    if (indicies.isComplete() &&
        devProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        devFeatures.geometryShader)
      return dev;
    if (indicies.isComplete() && devFeatures.geometryShader)
      potentialPhysDevice = dev; // if not discrete GPU but capable enough
  }
  return potentialPhysDevice; // if didn't find discrete GPU
}

VulkanInit::QueueFamilyIndicies
VulkanInit::findQueueFamilies(VkPhysicalDevice pDevice) {
  QueueFamilyIndicies indicies;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indicies.graphicsFamily = i;
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, mSurface,
                                           &presentSupport);
      if (presentSupport) {
        indicies.presentFamily = i;
      }

      if (indicies.isComplete())
        break;
    }
    i++;
  }

  return indicies;
}

void VulkanInit::createLogicalDevice() {
  mQFIndices = findQueueFamilies(mPhysicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamiles = {mQFIndices.graphicsFamily.value(),
                                           mQFIndices.presentFamily.value()};
  float queuePriority = 1.0f;
  for (auto queueFam : uniqueQueueFamiles) {
    VkDeviceQueueCreateInfo queueCreateInfo;
    zeroInitializeStruct(queueCreateInfo);
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = mQFIndices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }
  VkPhysicalDeviceFeatures deviceFeatures{};
  zeroInitializeStruct(deviceFeatures);

  VkDeviceCreateInfo createInfo{};
  zeroInitializeStruct(createInfo);
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;
  if (enabledValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }
  const char *devExtension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  createInfo.enabledExtensionCount = 1;
  createInfo.ppEnabledExtensionNames = &devExtension;

  if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice) !=
      VK_SUCCESS)
    throw std::runtime_error("Failed to create logical device!");
}
