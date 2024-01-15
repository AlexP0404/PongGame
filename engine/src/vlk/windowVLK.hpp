#ifndef WINDOW_VLK_HPP
#define WINDOW_VLK_HPP

#include <vector>
#include <vulkan/vulkan.h>

#include <string_view>
#include <vulkan/vulkan_core.h>

struct GLFWwindow;

class windowVLK {
public:
  windowVLK(int pWindowWidth, int pWindowHeight,
            const std::string_view &pWindowTitle);
  ~windowVLK();

  static void frameBufferResizeCallback(GLFWwindow *pWindow, int pWidth,
                                        int pHeight);
  void pollEvents();
  bool windowShouldClose();

  GLFWwindow *getWindowHandle();

private:
  void initWindow(int pWindowWidth, int pWindowHeight);
  void initVulkan();
  void cleanUpVulkan();
  void createInstance();
  inline const std::vector<const char *> getRequiredExtensions();
  bool checkValidationLayerSupport();
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);
  void setupDebugMessenger();
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

  template <typename T> void zeroInitializeStruct(T &a);

private:
  std::string_view mWindowTitle;
  bool mFrameBufferResized;
  GLFWwindow *mWindowHandle;

  VkInstance mInstance;
  std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  VkDebugUtilsMessengerEXT debugMessenger;
};

#endif
