# Vulkan Types

Dream's standard library provides minimal bindings for core Vulkan handles and structures. Handles are opaque 64-bit integers while enums mirror common Vulkan result codes.

```dream
// Creating instance information
VkApplicationInfo info = new VkApplicationInfo();
info.applicationName = "My App";
info.engineName = "DreamEngine";
info.apiVersion = 0x00400000; // Vulkan 1.4
```

These definitions allow Dream programs to prepare data for C functions that expect Vulkan-compatible structs.
