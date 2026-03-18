#include "vulkan_context.h"

namespace Engine {

  const std::vector<char const*> Vulkan_Context::validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };

  const std::vector<const char*> Vulkan_Context::deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

#ifdef NDEBUG
  constexpr b8 enableValidationLayers = false;
#else
  constexpr b8 enableValidationLayers = true;
#endif

  b8 Vulkan_Context::checkValidationLayerSupport() {

    u32 layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
      b8 layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  std::vector<const char*> getRequiredExtensions() {
    u32 glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData) {

    std::print("validation layer: {}\n", pCallbackData->pMessage);

    return VK_FALSE;
  }


  VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
      return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  void Vulkan_Context::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
      func(instance, debugMessenger, pAllocator);
    }
  }

  Vulkan_Context::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    Vulkan_Context::QueueFamilyIndices indices;

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
      }

      if (indices.isComplete()) {
        break;
      }
      i++;
    }
    return indices;
  }

  Vulkan_Context::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    Vulkan_Context::QueueFamilyIndices indices;
    u32 count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

    for (uint32_t i = 0; i < count; i++) {
      if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        indices.graphicsFamily = i;

      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
      if (presentSupport)
        indices.presentFamily = i;

      if (indices.isComplete()) break;
    }
    return indices;
  }

  bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    auto indices = findQueueFamilies(device, surface);
    return indices.isComplete();
  }

  void Vulkan_Context::createInstance()
  {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      std::print("validation layers requested, but not available!\n");
    }
    VkApplicationInfo appInfo{};
    appInfo.pApplicationName   = "undefined";
    appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.pEngineName        = "No Engine";
    appInfo.engineVersion      = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.apiVersion         = VK_API_VERSION_1_4;

    std::vector<char const*> requiredLayers;
    if (enableValidationLayers)
    {
      requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (enableValidationLayers) {
      createInfo.enabledLayerCount = static_cast<u32>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
      std::printf("failed to create instance!\n");
    }

    std::printf("available extensions\n");

    u32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    std::print("[1] Instance has been successfully created\n");
  }

  void Vulkan_Context::setupDebugMessenger()
  {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;

    if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debug_messenger) != VK_SUCCESS) {
      std::print("failed to set up debug messenger!\n");
    }
    std::print("[1.5] Debug Messenger has been successfully created\n");
  }

  void Vulkan_Context::pickPhysicalDevice()
  {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
      std::printf("failed to find GPUs with Vulkan support!\n");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
      if (isDeviceSuitable(device, m_surface)) {
        m_physical_device = device;
        break;
      }
    }

    if (m_physical_device == VK_NULL_HANDLE) {
      std::print("[FAIL] No suitable GPU found\n"); return; 
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_physical_device, &deviceProperties);
    std::print("[3] Physical device selected: {}\n", deviceProperties.deviceName);
  }

  void Vulkan_Context::createLogicalDevice()
  {
    m_queue_families = findQueueFamilies(m_physical_device, m_surface);
    QueueFamilyIndices indices = m_queue_families;

    f32 queuePriority = 1.0f;

    std::set<u32> uniqueFamilies = {
      indices.graphicsFamily.value(),
      indices.presentFamily.value()
    };

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (u32 family : uniqueFamilies) {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = family;
      queueCreateInfo.queueCount       = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo{};
    createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos       = queueCreateInfos.data();
    createInfo.queueCreateInfoCount    = static_cast<u32>(queueCreateInfos.size());
    createInfo.pEnabledFeatures        = &deviceFeatures;
    createInfo.enabledExtensionCount   = static_cast<u32>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
      createInfo.enabledLayerCount   = static_cast<u32>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physical_device, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
      std::print("[FAIL] vkCreateDevice\n"); return;
    }

    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, indices.presentFamily.value(),  0, &m_present_queue);
    std::print("[4] Device has been successfully created\n");
  }


  void Vulkan_Context::createSurface(Platform &platform_ctx) {
    if (glfwCreateWindowSurface(m_instance, platform_ctx.get_handle(), nullptr, &m_surface) != VK_SUCCESS) {
      throw std::printf("failed to create window surface\n");
    }
    std::print("[2] Surface has been successfully created\n");
  }

  Vulkan_Context::Vulkan_Context(Platform &platform)
  {
    createInstance();
    if (enableValidationLayers) {
      setupDebugMessenger();
    }
    createSurface(platform);
    pickPhysicalDevice();
    createLogicalDevice();
  }

  Vulkan_Context::~Vulkan_Context()
  {
    vkDestroyDevice(m_device, nullptr);
    if (enableValidationLayers)
      DestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
  }
}
