#include "sync.h"
#include <stdexcept>

namespace Engine {

  void Sync::create(VkDevice device, u32 image_count) {
    m_device = device;

    m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_render_finished_semaphores.resize(image_count); 
    m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (i32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device, &fence_info, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create sync objects");
        }
    }

    for (u32 i = MAX_FRAMES_IN_FLIGHT; i < image_count; i++) {
        if (vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create sync objects");
    }
}

void Sync::destroy() {
    if (m_device == VK_NULL_HANDLE) return;

    for (auto& s : m_image_available_semaphores)
        vkDestroySemaphore(m_device, s, nullptr);
    for (auto& s : m_render_finished_semaphores)
        vkDestroySemaphore(m_device, s, nullptr);
    for (auto& f : m_in_flight_fences)
        vkDestroyFence(m_device, f, nullptr);

    m_image_available_semaphores.clear();
    m_render_finished_semaphores.clear();
    m_in_flight_fences.clear();
    m_device = VK_NULL_HANDLE;
}

} // namespace helix
