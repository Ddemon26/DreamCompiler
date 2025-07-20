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

## Enumerating Physical Devices

The runtime provides a small helper to obtain the list of available physical devices without manual pointer juggling:

```dream
VkPhysicalDeviceList list = Vulkan.enumeratePhysicalDevices(instance);
Console.WriteLine(list.count);
```

The returned `VkPhysicalDeviceList` struct contains a pointer to the devices array and a `count` field. Memory for the array is allocated with `dr_alloc` and should be released with `dr_release` when no longer needed.
