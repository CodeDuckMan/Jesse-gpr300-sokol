#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"
#include "batteries/materials.h"

// ew
#include "ew/model.h"
#include "ew/mesh.h"
#include "ew/shader.h"
#include "ew/texture.h"

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
    std::unique_ptr<ew::Shader> water;

    std::unique_ptr<ew::Texture> water128; // mimap0
    std::unique_ptr<ew::Texture> water64; // mimap1
    std::unique_ptr<ew::Texture> water32; // mimap2
    std::unique_ptr<ew::Texture> water16; // mimap3
    std::unique_ptr<ew::Texture> water8; // mimap4


    ew::Mesh plane;

    batteries::ambient_t ambient;
    batteries::light_t light;
    batteries::material_t material;
};
