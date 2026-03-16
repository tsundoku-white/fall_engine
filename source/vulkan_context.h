#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <cstring>
#include <print>
#include <set>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "platform.h"

namespace Engine {

  class Vulkan_Context
  {
    public:
      struct QueueFamilyIndices
      {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
      };

      struct SwapChainSupportDetails
      {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
      };

      Vulkan_Context(Platform &platform);
      ~Vulkan_Context();

      VkInstance               get_instance()         const { return m_instance; }
      VkPhysicalDevice         get_physical_device()  const { return m_physical_device; }
      VkDevice                 get_device()           const { return m_device; }
      VkQueue                  get_graphics_queue()   const { return m_graphics_queue; }
      VkQueue                  get_present_queue()    const { return m_present_queue; }
      VkSurfaceKHR             get_surface()          const { return m_surface; }
      QueueFamilyIndices       get_queue_families()   const { return m_queue_families; }
    private:

      VkInstance m_instance                      = VK_NULL_HANDLE;
      VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;

      VkPhysicalDevice m_physical_device         = VK_NULL_HANDLE;
      VkDevice m_device                          = VK_NULL_HANDLE;
      VkQueue m_graphics_queue                   = VK_NULL_HANDLE;
      VkQueue m_present_queue                    = VK_NULL_HANDLE; 

      VkSurfaceKHR m_surface                     = VK_NULL_HANDLE;

      QueueFamilyIndices m_queue_families        = {};
      std::string m_gpu_name                     = "undefined";


      static const std::vector<const char*> validationLayers;
      static const std::vector<const char*> deviceExtensions;

      void createInstance();
      void setupDebugMessenger();
      void pickPhysicalDevice();
      void createLogicalDevice();
      void createSurface(Platform &platform_ctx);

      bool checkValidationLayerSupport();
      void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

  };
}
