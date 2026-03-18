#pragma once
#include "../common.h"

namespace Engine {

class Sync {
public:
    void create(VkDevice device, u32 image_count);
    void destroy();
    ~Sync() { destroy(); }

    VkSemaphore image_available(u32 frame) const { return m_image_available_semaphores[frame]; }
    VkSemaphore render_finished(u32 frame) const { return m_render_finished_semaphores[frame]; }
    VkFence     in_flight(u32 frame)       const { return m_in_flight_fences[frame]; }

    u32  current_frame() const { return m_current_frame; }
    void advance_frame()       { m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT; }

    static constexpr i32 MAX_FRAMES_IN_FLIGHT = 2;

private:
    VkDevice m_device = VK_NULL_HANDLE;

    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence>     m_in_flight_fences;

    u32 m_current_frame = 0;
};

} // namespace helix
