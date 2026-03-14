#include "platform.h"
#include "vulkan_context.h"
#include "render.h"
#include <cstdio>
#include <print>

int main(int argc, char *argv[])
{
  helix::Platform platform;
  helix::Vulkan_Context vulkan_ctx(platform);
  helix::Render render(vulkan_ctx, platform);

  while (!platform.should_close())
  {
    platform.pollevents(); 

    if (platform.input_pressed(GLFW_KEY_F3)) break;

    //float foo = platform.input_vector2(GLFW_KEY_A, GLFW_KEY_D, platform.delta);
    //std::printf("value: %f\n", foo);
    render.pass();
  }
}
