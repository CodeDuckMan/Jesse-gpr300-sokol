#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

#include "ew/procGen.h"

struct {
    float alpha = 128.0f;
    glm::vec3 waterColor{0.5f, 0.5f, 0.5f};
} debug;



Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    water = std::make_unique<ew::Shader>("assets/shaders/water.vs", "assets/shaders/water.fs");
    water128 = std::make_unique<ew::Texture>("assets/textures/windwaker/water128");
    water64 = std::make_unique<ew::Texture>("assets/textures/windwaker/water64");
    water32 = std::make_unique<ew::Texture>("assets/textures/windwaker/water32");
    water16 = std::make_unique<ew::Texture>("assets/textures/windwaker/water16");
    water8 = std::make_unique<ew::Texture>("assets/textures/windwaker/water8");

    plane.load(ew::createPlane(100.0f, 100.0f, 100.0f));
}

Scene::~Scene()
{

}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}


void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);

    // Set Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, water128->getID());

    water->use();

    // Sample the texture
    water->setInt("texture0", 0);

    // scene matrices
    water->setMat4("model", glm::mat4(1.0f));
    water->setMat4("view_proj", view_proj);

    water->setVec3("camera_position", camera.position);

    water->setFloat("time", (float)time.absolute);
    water->setVec3("WaterColor", debug.waterColor);


    // draw plane
    plane.draw();
}

void Scene::Debug(void)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    glm::mat4 m{1.0f};
    auto *view = glm::value_ptr(camera.View());
    auto *proj = glm::value_ptr(camera.Projection());
    
    auto light_matrix = glm::translate(glm::mat4(1.0f),light.position);

    
    ImGuizmo::DrawGrid(view, proj, glm::value_ptr(m), 100.0f);

    ImGuizmo::Manipulate(
        view,
        proj,
        ImGuizmo::TRANSLATE,
        ImGuizmo::WORLD,
        glm::value_ptr(light_matrix)
    );

    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if(ImGuizmo::IsUsing){
        light.position = glm::vec3(light_matrix[3]);
    }

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    //Imgui::DragFloat("Alpha", &debug.alpha, 1, 100);
    //Imgui::ColorEdit3("Light Color:", &light.color);

    /* build debug ui here */

    ImGui::End();
}