#ifndef WINDOW_VLK_HPP
#define WINDOW_VLK_HPP

#include "vulkanInit.hpp"
#include "vulkanRenderData.hpp"

#include <memory>
#include <string_view>

struct GLFWwindow;

class WindowVLK {
public:
  explicit WindowVLK(const std::string_view &pWindowTitle);
  WindowVLK(int pWindowWidth, int pWindowHeight,
            const std::string_view &pWindowTitle);
  ~WindowVLK();

  void initVulkan();

  void pollEvents();
  bool windowShouldClose();

  GLFWwindow *getWindowHandle();

private:
  void initWindow(int pWindowWidth, int pWindowHeight);
  static void frameBufferResizeCallback(GLFWwindow *pWindow, int pWidth,
                                        int pHeight);

private:
  std::string_view mWindowTitle;
  bool mFrameBufferResized;
  GLFWwindow *mWindowHandle;
  std::shared_ptr<VulkanInit> mVLKInit;
  VulkanRenderData mVLKData;
};

#endif
