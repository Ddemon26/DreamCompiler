#include "vulkan.h"
#include "memory.h"
#include <string.h>
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
