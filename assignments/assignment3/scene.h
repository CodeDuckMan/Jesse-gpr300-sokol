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
    std::unique_ptr<ew::Model> skull;

    std::unique_ptr<ew::Texture> texture;

    std::unique_ptr<ew::Shader> blinnphong;
    std::unique_ptr<ew::Shader> fullscreen; // Put on timeout for turning the world red
    // std::unique_ptr<ew::Shader> inverse;
    // std::unique_ptr<ew::Shader> grescale;
    // std::unique_ptr<ew::Shader> blur;
    // std::unique_ptr<ew::Shader> edges;

    std::vector<std::unique_ptr<ew::Shader>> postProcessingEffects;
    batteries::ambient_t ambient;
    batteries::light_t light;

    std::vector<glm::mat4> modelInstances;

    GLuint fbo;
    GLuint fbo_depth;
    GLuint fbo_texture;

    GLuint shadow_fbo;
    GLuint shadow_vbo;


};
