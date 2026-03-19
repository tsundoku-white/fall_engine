#include "platform.h"
#include "vulkan_context.h"
#include "drivers/render.h"

int main()
{
    Engine::Platform platform;
    platform.set_window_mode(Engine::WindowModeWindowed);
    Engine::Vulkan_Context ctx(platform);
    Engine::Render render(ctx, platform);

    while (!platform.should_close()) {
        platform.pollevents();
        render.pass();
    }

    return 0;
}
