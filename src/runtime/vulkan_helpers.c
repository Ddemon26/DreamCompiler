#include "vulkan_helpers.h"
#include "memory.h"
#include <vulkan/vulkan.h>

DrVkDeviceList dr_vk_enumerate_physical_devices(VkInstance instance) {
  DrVkDeviceList list = {0};
  uint32_t count = 0;
  if (vkEnumeratePhysicalDevices(instance, &count, NULL) != VK_SUCCESS ||
      count == 0)
    return list;
  list.devices = dr_alloc(sizeof(VkPhysicalDevice) * count);
  if (!list.devices)
    return list;
  if (vkEnumeratePhysicalDevices(instance, &count, list.devices) !=
      VK_SUCCESS) {
    dr_release(list.devices);
    list.devices = NULL;
    return list;
  }
  list.count = count;
  return list;
}
