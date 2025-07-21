#ifndef DR_VULKAN_STUB_H
#define DR_VULKAN_STUB_H

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

int dr_vulkan_available(void);
uint32_t dr_vulkan_version(void);
PFN_vkVoidFunction dr_load_instance_proc(VkInstance instance, const char *name);
PFN_vkVoidFunction dr_load_device_proc(VkDevice device, const char *name);

#ifdef __cplusplus
}
#endif

#endif // DR_VULKAN_STUB_H
