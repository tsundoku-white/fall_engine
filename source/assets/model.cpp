#include "model.h"
#include <cstdio>



void Model::create(string path)
{
  cgltf_options options = {};
  cgltf_data* data = nullptr;
  cgltf_result result = cgltf_parse_file(&options, path.c_str() , &data);
  if (result == cgltf_result_success)
  { 
    cgltf_free(data);
  }
}

void Model::destroy()
{

}
