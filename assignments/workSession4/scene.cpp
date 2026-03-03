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
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    toon = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/toon.fs");
    skullTexture = std::make_unique<ew::Texture>("assets/textures/Txo_dokuo.png");
    // light = { 
    //     .color = {1.0f, 0.0f, 2.0f}, }
    //     .position = {2.0f, 2.0f, 2.0f}, 

    palette = {
        .color1{1.0f, 1.0f, 1.0f},
        .color2{0.0f, 0.0f, 0.0f},
    };

    // // frame buffer setup
    // glCreateFramebuffers(1, &fbo);
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // {
    //     glGenTextures(1, &fboTexture);
    //     glBindTexture(GL_TEXTURE_2D, fboTexture);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // }
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NULL);
    // if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    // {
    //     printf("Framebuffer not complete");
    // }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
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


    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);

    GLuint intSkullTexture = skullTexture.get()->getID();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, intSkullTexture);


    toon->use();

    // scene matrices
    toon->setMat4("model", glm::mat4(1.0f));
    toon->setMat4("view_proj", view_proj);
    
    toon->setVec3("camera_position", camera.position);
    toon->setVec3("light", light.position);
    toon->setVec3("light_color", light.color);

    toon->setVec3("Light.color", light.color);
    toon->setVec3("Light.position", light.position);
    
    toon->setVec3("Material.ambient", light.color);
    toon->setVec3("Material.deffuse", light.position);
    toon->setVec3("Material.specular", light.color);
    toon->setVec3("Material.shinniness", light.color);


    // // Call texture bindings before draw
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE0)

    // draw suzanne
    suzanne->draw();
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
// Scene::Scene()
// {
//     suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
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

//     // draw suzanne
//     suzanne->draw();
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
