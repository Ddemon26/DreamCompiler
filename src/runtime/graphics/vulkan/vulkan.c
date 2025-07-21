#include "../vulkan.h"
#include "memory.h"
#include <string.h>
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#elif defined(__linux__)
#define VK_USE_PLATFORM_XCB_KHR
#include <xcb/xcb.h>
#endif
#include <vulkan/vulkan.h>

VkResult dr_vkCreateInstance(const DrVkInstanceCreateInfo *info,
                             VkInstance *outInstance) {
  if (!info || !outInstance)
    return VK_ERROR_INITIALIZATION_FAILED;

  VkApplicationInfo appInfo = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = NULL,
      .pApplicationName = info->appInfo ? info->appInfo->applicationName : NULL,
      .pEngineName = info->appInfo ? info->appInfo->engineName : NULL,
      .applicationVersion = 0,
      .engineVersion = 0,
      .apiVersion =
          info->appInfo ? info->appInfo->apiVersion : VK_API_VERSION_1_0,
  };

  VkInstanceCreateInfo ci = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .pApplicationInfo = info->appInfo ? &appInfo : NULL,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = NULL,
      .enabledExtensionCount = info->enabledExtensionCount,
      .ppEnabledExtensionNames =
          (const char *const *)info->ppEnabledExtensionNames,
  };

  return vkCreateInstance(&ci, NULL, outInstance);
}

void dr_vkDestroyInstance(VkInstance instance) {
  if (instance)
    vkDestroyInstance(instance, NULL);
}

VkResult dr_vkCreateSurface(VkInstance instance,
                            const DrVkSurfaceCreateInfo *info,
                            VkSurfaceKHR *outSurface) {
  if (!info || !outSurface)
    return VK_ERROR_INITIALIZATION_FAILED;
#ifdef _WIN32
  VkWin32SurfaceCreateInfoKHR ci = {
      .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
      .pNext = NULL,
      .flags = 0,
      .hinstance = (HINSTANCE)info->hinstance,
      .hwnd = (HWND)info->hwnd,
  };
  PFN_vkCreateWin32SurfaceKHR pfn =
      (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(
          instance, "vkCreateWin32SurfaceKHR");
  if (!pfn)
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  return pfn(instance, &ci, NULL, outSurface);
#elif defined(__linux__)
  VkXcbSurfaceCreateInfoKHR ci = {
      .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
      .pNext = NULL,
      .flags = 0,
      .connection = (xcb_connection_t *)info->connection,
      .window = info->window,
  };
  PFN_vkCreateXcbSurfaceKHR pfn =
      (PFN_vkCreateXcbSurfaceKHR)vkGetInstanceProcAddr(
          instance, "vkCreateXcbSurfaceKHR");
  if (!pfn)
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  return pfn(instance, &ci, NULL, outSurface);
#else
  (void)instance;
  (void)info;
  (void)outSurface;
  return VK_ERROR_EXTENSION_NOT_PRESENT;
#endif
}

void dr_vkDestroySurface(VkInstance instance, VkSurfaceKHR surface) {
  if (!surface)
    return;
  PFN_vkDestroySurfaceKHR pfn =
      (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(
          instance, "vkDestroySurfaceKHR");
    if (pfn)
      pfn(instance, surface, NULL);
}

VkResult dr_vkCreateSimpleSwapchain(VkDevice device, VkSurfaceKHR surface,
                                    uint32_t width, uint32_t height,
                                    VkSwapchainKHR *outSwapchain) {
  if (!outSwapchain)
    return VK_ERROR_INITIALIZATION_FAILED;

  VkSwapchainCreateInfoKHR ci = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = NULL,
      .flags = 0,
      .surface = surface,
      .minImageCount = 2,
      .imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
      .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
      .imageExtent = {width, height},
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = NULL,
      .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = VK_PRESENT_MODE_FIFO_KHR,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  PFN_vkCreateSwapchainKHR pfn =
      (PFN_vkCreateSwapchainKHR)vkGetDeviceProcAddr(device,
                                                    "vkCreateSwapchainKHR");
  if (!pfn)
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  return pfn(device, &ci, NULL, outSwapchain);
}

void dr_vkDestroySwapchain(VkDevice device, VkSwapchainKHR swapchain) {
  if (!swapchain)
    return;
  PFN_vkDestroySwapchainKHR pfn =
      (PFN_vkDestroySwapchainKHR)vkGetDeviceProcAddr(device,
                                                     "vkDestroySwapchainKHR");
  if (pfn)
    pfn(device, swapchain, NULL);
}
