#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#elif defined(__linux__)
#define VK_USE_PLATFORM_XCB_KHR
#include <xcb/xcb.h>
#endif

#include <vulkan/vulkan.h>
#include "../vulkan_stub.h"

int dr_vulkan_available(void) {
    return vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkGetInstanceProcAddr") != NULL;
}

uint32_t dr_vulkan_version(void) {
    PFN_vkEnumerateInstanceVersion pfn =
        (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion");
    if (!pfn) return 0;
    uint32_t version = 0;
    if (pfn(&version) != VK_SUCCESS) return 0;
    return version;
}

PFN_vkVoidFunction dr_load_instance_proc(VkInstance instance, const char *name) {
    return vkGetInstanceProcAddr(instance, name);
}

PFN_vkVoidFunction dr_load_device_proc(VkDevice device, const char *name) {
    return vkGetDeviceProcAddr(device, name);
}
