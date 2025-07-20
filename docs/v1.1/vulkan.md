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

## Creating and Destroying an Instance

Dream exposes wrappers for `vkCreateInstance` and `vkDestroyInstance` through the `Vulkan` class. Instance creation mirrors the C API closely:

```dream
VkInstance inst;
VkResult res = Vulkan.createInstance(&info, &inst);
if (res != VkResult.Success) {
  Console.WriteLine("Failed to create instance");
}
// ... use the instance ...
Vulkan.destroyInstance(inst);
```

Before calling any Vulkan function you can check availability and version using `Vulkan.available()` and `Vulkan.version()`.

## Creating Devices and Buffers

More advanced wrappers directly expose core Vulkan entry points for device and buffer management:

```dream
VkDevice dev;
Vulkan.createDevice(list.devices[0], &deviceInfo, null, &dev);

VkBuffer buf;
Vulkan.createBuffer(dev, &bufferInfo, null, &buf);
// allocate and bind memory...
Vulkan.destroyBuffer(dev, buf, null);
Vulkan.destroyDevice(dev, null);
```

These APIs retain Vulkan's low-level behaviour, requiring callers to manage memory and error codes just like the native C interface.

## Creating Window Surfaces

To present to a window you must create a `VkSurfaceKHR` for your platform. `Vulkan.createSurface` wraps the platform-specific calls and accepts a `VkSurfaceCreateInfo` structure with native handles:

```dream
VkSurfaceCreateInfo surfInfo = new VkSurfaceCreateInfo();
surfInfo.handle1 = getPlatformInstance(); // HINSTANCE or xcb_connection_t*
surfInfo.handle2 = getPlatformWindow();   // HWND or xcb_window_t

VkSurfaceKHR surface;
VkResult res = Vulkan.createSurface(inst, &surfInfo, &surface);
```

Destroy a surface when finished:

```dream
Vulkan.destroySurface(inst, surface);
```

## Convenience Helpers

To reduce boilerplate the standard module offers small helper functions.

### Picking the First Device

```dream
VkPhysicalDevice dev = Vulkan.pickFirstPhysicalDevice(inst);
if (dev.value == 0) {
  Console.WriteLine("No GPU available");
}
```

### Creating a Default Swapchain

```dream
VkSwapchainKHR swap;
Vulkan.createDefaultSwapchain(device, surface, 800, 600, &swap);
// ... use the swapchain ...
Vulkan.destroySwapchain(device, swap);
```

## Shader Modules and Pipeline Layouts

The module also exposes low-level entry points for building graphics pipelines.
Shader modules are created directly from SPIR-V bytecode using
`Vulkan.createShaderModule`:

```dream
VkShaderModuleCreateInfo smInfo = new VkShaderModuleCreateInfo();
smInfo.codeSize = spirvSize;
smInfo.pCode = spirvPtr;
VkShaderModule shader;
VkResult res = Vulkan.createShaderModule(device, &smInfo, null, &shader);
```

Descriptor set layouts and pipeline layouts mirror the C API closely:

```dream
VkDescriptorSetLayoutBinding bind = new VkDescriptorSetLayoutBinding();
bind.binding = 0;
bind.descriptorType = 0; // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
bind.descriptorCount = 1;
bind.stageFlags = 1;     // VK_SHADER_STAGE_VERTEX_BIT

VkDescriptorSetLayoutCreateInfo dl = new VkDescriptorSetLayoutCreateInfo();
dl.bindingCount = 1;
dl.pBindings = &bind;
VkDescriptorSetLayout setLayout;
Vulkan.createDescriptorSetLayout(device, &dl, null, &setLayout);

VkPipelineLayoutCreateInfo pl = new VkPipelineLayoutCreateInfo();
pl.setLayoutCount = 1;
pl.pSetLayouts = &setLayout;
pl.pushConstantRangeCount = 0;
pl.pPushConstantRanges = 0;
VkPipelineLayout layout;
Vulkan.createPipelineLayout(device, &pl, null, &layout);
```

Destroy these objects with `destroyShaderModule`, `destroyDescriptorSetLayout`
and `destroyPipelineLayout` when no longer needed.
