#pragma once

// NOT FINISHED SO IT IS NOT INCLUDED IN CMAKE WILL BE FINISHED LATER

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "../drivers/buffer.h"
#include "../common.h"

class Model 
{
  public:
    void create(string path);
    void destroy();
    ~Model() { destroy(); }
  private:
};

