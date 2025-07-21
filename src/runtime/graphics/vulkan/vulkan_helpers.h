#ifndef DR_VULKAN_HELPERS_H
#define DR_VULKAN_HELPERS_H

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DrVkDeviceList {
  VkPhysicalDevice *devices;
  uint32_t count;
} DrVkDeviceList;

DrVkDeviceList dr_vk_enumerate_physical_devices(VkInstance instance);

#ifdef __cplusplus
}
#endif

#endif // DR_VULKAN_HELPERS_H
