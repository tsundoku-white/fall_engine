#include "platform.h"
#include "vulkan_context.h"
#include "drivers/render.h"

int main()
{
    Engine::Platform platform;
    Engine::Vulkan_Context ctx(platform);

    Engine::Render render(ctx, platform);

    while (!platform.should_close()) {
        platform.pollevents();
        render.pass();
    }

    return 0;
}
