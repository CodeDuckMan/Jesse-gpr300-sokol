#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

struct
{
    int index = 0;
    int currentPostProc = 0;
    float blurStrength = 16.0f;
    float spawnArea = 3;
    float spacing = 1;

    struct
    {
        glm::vec3 offset = glm::vec3(0.009f, 0.006f, -0.006f);
        glm::vec2 direction = glm::vec2(1.0f);
    } chromatic;

    struct
    {
        int x = 800;
        int y = 600;
    } window;

    

} settings;

    static std::vector<std::string> postProcessingNames =
    {
        "None",
        "Blur",
        "Inverse",
        "Greyscle",
        "Edges",
    };

struct FullScreenQuad
{
    GLuint vao;
    GLuint vbo;

    void Initialize()
    {
        float vertices[] = {
            // pos (x, y),
            // texcoord (u, v)
            // triangle 1
            -1.0f, 1.0f, 0.0f, 1.0f,   
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            // triangle 2
            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        // pos (x, y),
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // texcoord (u, v)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

        // always last.
        glBindVertexArray(0);


    }
};

FullScreenQuad fullscreenQuad;

struct FrameBuffer
{
    GLuint fbo;
    GLuint color0;
    GLuint color1;
    GLuint depth;
    
    void Initialize()
    {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // create a color attachment texture
    glGenTextures(1, &color0);
    glBindTexture(GL_TEXTURE_2D, color0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, settings.window.x, settings.window.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color0, 0);
    
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    
    /* old method
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, settings.window.x, settings.window.y); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);
    */

    // new method -> ceate depth as texture
    
        glGenTextures(1, &depth);
        glBindTexture(GL_TEXTURE_2D, depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, settings.window.x, settings.window.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

};

FrameBuffer frameBuffer;

void doPostProcess(ew::Shader* shader)
{
    shader->use();
    shader->setInt("texture0", 0);

    if (settings.currentPostProc == 1)
    {
        shader->setFloat("strength", settings.blurStrength);
    }

    // Disable depth test
    glDisable(GL_DEPTH_TEST);

    // Clear buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw 
    glBindVertexArray(fullscreenQuad.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frameBuffer.color0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CacheInstanceData()
{
    auto i = 0;

    auto width = settings.spawnArea;
    auto size = (width - (-width) + 1) * (width - (width) + 1);

    for (auto x = -settings.spawnArea; x <= settings.spawnArea; x++)
    {
        for (auto y = -settings.spawnArea; y <= settings.spawnArea; y++, i++)
        {
            auto position = glm::vec3(x * settings.spacing, 0, y * settings.spacing, 0);
            auto matrix = glm::translate(glm::mat4(1.0f), position);
        }
    }
}

Scene::Scene()
{
    skull = std::make_unique<ew::Model>("assets/models/skull.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/blinnphong.vs", "assets/shaders/blinnphong.fs");
    texture = std::make_unique<ew::Texture>("assets/textures/Txo_dokuo.png");
    
    // blur = std::make_unique<ew::Shader>("assets/shaders/PostProcessing/default.vs", "assets/shaders/PostProcessing/blur.fs");
    // edges = std::make_unique<ew::Shader>("assets/shaders/PostProcessing/default.vs", "assets/shaders/PostProcessing/edges.fs");
    // grescale = std::make_unique<ew::Shader>("assets/shaders/PostProcessing/default.vs", "assets/shaders/PostProcessing/greyscale.fs");
    // inverse = std::make_unique<ew::Shader>("assets/shaders/PostProcessing/default.vs", "assets/shaders/PostProcessing/inverse.fs");
    
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/postprocessing/default.vs", "assets/shaders/postprocessing/default.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcessing/default.vs", "assets/shaders/PostProcessing/blur.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcessing/default.vs", "assets/shaders/PostProcessing/edges.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcessing/default.vs", "assets/shaders/PostProcessing/greyscale.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcessing/default.vs", "assets/shaders/PostProcessing/inverse.fs"));
    //fullscreen = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/fullscreen.fs");
    
    light = {  
         .color = {1.0f, 0.0f, 2.0f}, 
         .position = {2.0f, 2.0f, 2.0f}, 
     };

     ambient = {
        .intensity = 1.0f,
        .color = {0.5,0.5,0.5},
     };

     
    
     // create fbo
     // https://learnopengl.com/Advanced-OpenGL/Framebuffers
    //  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, settings.window.x, settings.window.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
     // Do not Render buffer objects. Create a color texture and then a depth texture
    //  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, settings.window.x, settings.window.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    frameBuffer.Initialize();
    fullscreenQuad.Initialize();

};

Scene::~Scene()
{
    // delete framebuffer
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}


void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    {   // Framebuffer
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

        // Scene matrices
        blinnphong->setMat4("model", glm::mat4(1.0f));
        blinnphong->setMat4("view_proj", view_proj);

        blinnphong->setVec3("cameraPosition", camera.position);
        blinnphong->setVec3("lightPosition", light.position);
        blinnphong->setVec3("light_color", light.color);

        blinnphong->setVec3("lightStruct.color", light.color);
        blinnphong->setVec3("lightStruct.position", light.position);

        // Draw
        skull->draw();
        
        auto i = 0;
    for (auto x = -settings.spawnArea; x <= settings.spawnArea; x++)
    {
        for (auto y = -settings.spawnArea; y <= settings.spawnArea; y++, i++)
        {
            auto position = glm::vec3(x * settings.spacing, 0, y * settings.spacing, 0);
            auto matrix = glm::translate(glm::mat4(1.0f), position);
        }
    }

        doPostProcess(postProcessingEffects[settings.currentPostProc].get());
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Postprocessing

    // Smaller fullscrean window
    // {
    //     fullscreen->use();
    //     fullscreen->setInt("screen", 0);

    //     // Fullscreen pipeline
    //     glDisable(GL_DEPTH_TEST);

    //     // default frame buffer
    //     glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //     // draw fullscreenquad
    //     glBindVertexArray(fullscreen_quad.vao);
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, fbo_texture);
    //     glDrawArrays(GL_TRIANGLES, 0, 6);
    // }
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

    if(ImGuizmo::IsUsing)
    {
        light.position = glm::vec3(light_matrix[3]);
    }

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);
    ImGui::SliderFloat("Width", &settings.spawnArea, 0.0f, 10.0f);

    //Imgui::DragFloat("Alpha", &debug.alpha, 1, 100);
    //Imgui::ColorEdit3("Light Color:", &light.color);

    /* build debug ui here */

    if (ImGui::BeginCombo("Effect", postProcessingNames[settings.currentPostProc].c_str()))    
    {
            for (auto n = 0; n < postProcessingNames.size(); ++n)
            {
                auto is_selected = (postProcessingNames[settings.currentPostProc] == postProcessingNames[n]);
                
                if (ImGui::Selectable(postProcessingNames[n].c_str(), is_selected))
                {
                    settings.currentPostProc = n;
                }
                
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
                
                
            }
        
            ImGui::EndCombo();
    }
        
    
    // ImGui::Image(
    //     (void*)(intptr_t)fbo_texture,
    //     ImVec2(400, 300),
    //     ImVec2(0, 1), ImVec2(1, 0));

    // ImGui::Image(
    //     (void*)(intptr_t)fbo_depth,
    //     ImVec2(400, 300),
    //     ImVec2(0, 1), ImVec2(1, 0));

    
    ImGui::End();
}