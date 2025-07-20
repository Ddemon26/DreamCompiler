#include <vulkan/vulkan_core.h>
#include "vulkan_stub.h"

int dr_vulkan_available(void) {
    return vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkGetInstanceProcAddr") != NULL;
}
