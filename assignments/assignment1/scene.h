#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"

// ew
#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"

#include "batteries/opengl.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);

  private:
    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Shader> blinnphong;
    std::unique_ptr<ew::Shader> postprocess;
    std::unique_ptr<ew::Texture> texture;

    batteries::ambient_t ambient;
    batteries::light_t light;

    GLuint fbo;
    GLuint fbo_color0;
    GLuint fbo_depth;
};
