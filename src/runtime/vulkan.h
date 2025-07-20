#ifndef DR_VULKAN_H
#define DR_VULKAN_H

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DrVkApplicationInfo {
  char *applicationName;
  char *engineName;
  uint32_t apiVersion;
} DrVkApplicationInfo;

typedef struct DrVkInstanceCreateInfo {
  DrVkApplicationInfo *appInfo;
  uint32_t enabledExtensionCount;
  char **ppEnabledExtensionNames;
} DrVkInstanceCreateInfo;

typedef struct DrVkSurfaceCreateInfo {
#ifdef _WIN32
  void *hinstance;
  void *hwnd;
#elif defined(__linux__)
  void *connection;
  uint32_t window;
#else
  void *unused1;
  void *unused2;
#endif
} DrVkSurfaceCreateInfo;

VkResult dr_vkCreateInstance(const DrVkInstanceCreateInfo *info,
                             VkInstance *outInstance);
void dr_vkDestroyInstance(VkInstance instance);
VkResult dr_vkCreateSurface(VkInstance instance,
                            const DrVkSurfaceCreateInfo *info,
                            VkSurfaceKHR *outSurface);
void dr_vkDestroySurface(VkInstance instance, VkSurfaceKHR surface);

VkResult dr_vkCreateSimpleSwapchain(VkDevice device,
                                    VkSurfaceKHR surface,
                                    uint32_t width,
                                    uint32_t height,
                                    VkSwapchainKHR *outSwapchain);
void dr_vkDestroySwapchain(VkDevice device, VkSwapchainKHR swapchain);

#ifdef __cplusplus
}
#endif

#endif // DR_VULKAN_H
