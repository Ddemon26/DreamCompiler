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

VkResult dr_vkCreateInstance(const DrVkInstanceCreateInfo *info,
                             VkInstance *outInstance);
void dr_vkDestroyInstance(VkInstance instance);

#ifdef __cplusplus
}
#endif

#endif // DR_VULKAN_H
