#pragma once 

#include "glm/ext/matrix_float4x4.hpp"
#include <glm/gtx/quaternion.hpp>
class Camera
{
  public:
  glm::vec3 location = {0,0,0};
  glm::quat rotation = {1,0,0,0};

  float fov = 90.0f;

  void update();
  void set_rotation(glm::vec3 rot);
  void set_rotationf(float pitch, float yaw, float roll);
  void set_location(glm::vec3 loc);
  
  private:
  glm::mat4 projection;
  glm::mat4 view;

  glm::vec3 up = {0,1,0};
  glm::vec3 right = {1, 0, 0};
  glm::vec3 forward = {0,0,-1};

  void projection_update();
  void compute_rotation();
};
