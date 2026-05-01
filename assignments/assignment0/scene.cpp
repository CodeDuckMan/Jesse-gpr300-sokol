#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

struct {
    float alpha = 128.0f;
} debug;



Scene::Scene()
{
    skull = std::make_unique<ew::Model>("assets/models/skull.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/blinnphong.vs", "assets/shaders/blinnphong.fs");
    texture = std::make_unique<ew::Texture>("assets/textures/Txo_dokuo.png");

     light = {  
         .color = {1.0f, 0.0f, 2.0f}, 
         .position = {2.0f, 2.0f, 2.0f}, 
     };

     ambient = {
        .intensity = 1.0f,
        .color = {0.5,0.5,0.5},
     };

     

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
    glBindTexture(GL_TEXTURE_2D, texture->getID());

    blinnphong->use();

    // Sample the texture
    blinnphong->setInt("texture0", 0);

    // scene matrices
    blinnphong->setMat4("model", glm::mat4(1.0f));
    blinnphong->setMat4("view_proj", view_proj);

    blinnphong->setVec3("camera_position", camera.position);
    blinnphong->setVec3("lightPosition", light.position);
    blinnphong->setVec3("light_color", light.color);

    blinnphong->setVec3("lightStruct.color", light.color);
    blinnphong->setVec3("lightStruct.position", light.position);

    


    // draw skull
    skull->draw();
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
    //ImGui::DragFloat("Alpha", &debug.alpha, 1, 100);
    //ImGui::ColorEdit3("Light Color", &light.color.r);
    //Imgui::ColorEdit3("Light Color:", &light.color);

    /* build debug ui here */

    ImGui::End();
}
// Scene::Scene()
// {
//     skull = std::make_unique<ew::Model>("assets/models/skull.obj");
//     blinnphong = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/blinnphong.fs");

//     // light = { 
//     //     .color = {1.0f, 0.0f, 2.0f}, }
//     //     .position = {2.0f, 2.0f, 2.0f}, 
// }

// Scene::~Scene()
// {
// }

// void Scene::Update(float dt)
// {
//     batteries::Scene::Update(dt);

//     /* body */
// }


// void Scene::Render(void)
// {
//     const auto view_proj = camera.Projection() * camera.View();

//     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     glEnable(GL_CULL_FACE);
//     glCullFace(GL_BACK);
//     glEnable(GL_DEPTH_TEST);
//     // glDisable(GL_DEPTH_TEST);

//     blinnphong->use();

//     // scene matrices
//     blinnphong->setMat4("model", glm::mat4(1.0f));
//     blinnphong->setMat4("view_proj", view_proj);

//     blinnphong->setVec3("camera", camera.position);
//     blinnphong->setVec3("light", light.position);
//     blinnphong->setVec3("light_color", light.color);
//     // blinnphong->setVec3("light_color", light.color);

//     blinnphong->setVec3("Light.color", light.color);
//     blinnphong->setVec3("Light.position", light.position);
    
//     blinnphong->setVec3("Material.ambient", light.color);
//     blinnphong->setVec3("Material.deffuse", light.position);
//     blinnphong->setVec3("Material.specular", light.color);
//     blinnphong->setVec3("Material.shinniness", light.color);

//     // draw skull
//     skull->draw();
// }

// void Scene::Debug(void)
// {
//     ImGuizmo::BeginFrame();
//     ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
//     ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

//     glm::mat4 m{1.0f};
//     auto *view = glm::value_ptr(camera.View());
//     auto *proj = glm::value_ptr(camera.Projection());
    
//     auto light_matrix = glm::translate(glm::mat4(1.0f),light.position);

    
//     ImGuizmo::DrawGrid(view, proj, glm::value_ptr(m), 100.0f);

//     ImGuizmo::Manipulate(
//         view,
//         proj,
//         ImGuizmo::TRANSLATE,
//         ImGuizmo::WORLD,
//         glm::value_ptr(light_matrix)
//     );

//     cameracontroller.Debug();

//     ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

//     if(ImGuizmo::IsUsing){
//         light.position = glm::vec3(light_matrix[3]);
//     }

//     ImGui::Checkbox("Paused", &time.paused);
//     ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

//     //Imgui::DragFloat("Alpha", &debug.alpha, 1, 100);
//     //Imgui::ColorEdit3("Light Color:", &light.color);

//     /* build debug ui here */

//     ImGui::End();
// }
