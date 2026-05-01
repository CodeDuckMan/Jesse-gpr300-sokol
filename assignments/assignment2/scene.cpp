#include "scene.h"
// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"
#include "batteries/materials.h"
#include "batteries/math.h"


enum
{
    DefaultShader = 0,
    DepthShader = 1,
    ShadowShader = 2,
};

struct
{
    int index = 0;
    int currentPostProc = 0;
    float blurStrength = 16.0f;
    
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

    struct
    {
        glm::vec4 lightRadius = {2.0f, 2.0f, -2.0f, 1.0f};
    } lights;

        
    struct Material
    {
        glm::vec3 ambient{1.0f};
        glm::vec3 diffuse{0.5f};
        glm::vec3 specular{0.5f};
        float shininess = 0.5f;
    } material;

    float bias = 1.0f;
    bool cull_front = false;
    bool use_pcf = false;

} settings;

    static std::vector<std::string> postProcessingNames =
    {
        "Default",
        "Depth",
        "Shadow",
    };

struct DepthBuffer{
    GLuint fbo;
    GLuint depth;

    void Initialize(){
        glGenFramebuffers(1, &fbo);
        
        glGenTextures(1, &depth);
        glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, settings.window.x, settings.window.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

        glDrawBuffers(0, nullptr);
        glReadBuffer(GL_NONE);

        // Check buffer
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("DepthBuffer failed to initialize\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

};

DepthBuffer depthBuffer;

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, settings.window.x, settings.window.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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


Scene::Scene()
{
    
    skull = std::make_unique<ew::Model>("assets/models/skull.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/shadow.vs", "assets/shaders/shadow.fs");
    
    texture = std::make_unique<ew::Texture>("assets/textures/Txo_dokuo.png");
    depth = std::make_unique<ew::Shader>("assets/shaders/depth.vs", "assets/shaders/depth.fs");


    light = {  
         .brightness = 10.0f,
         .color = {1.0f, 1.0f, 1.0f}, 
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

    
    depthBuffer.Initialize();
    fullscreenQuad.Initialize();

    planeMesh.load(ew::createPlane(500.0f, 500.0f, 1));
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

    const auto light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    const auto light_view = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    const auto light_view_proj = light_proj * light_view;

    glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer.fbo);
    {   
        // Shadowbuffer
        glEnable(GL_CULL_FACE);
        glCullFace(settings.cull_front ? GL_FRONT : GL_BACK);
        glEnable(GL_DEPTH_TEST);
            
        glViewport(0, 0, settings.window.x, settings.window.y);
        
        glClear(GL_DEPTH_BUFFER_BIT);

        depth->use();

        depth->setMat4("model", glm::mat4(1.0f));
        depth->setMat4("light_view_proj", light_view_proj);
        
        skull->draw();
    }
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
        
    glViewport(0, 0, sapp_width(), sapp_height());

    // Set Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthBuffer.depth);


    blinnphong->use();

    // Sample the texture
    blinnphong->setInt("shadow_map", 0);

    // Scene matrices
    blinnphong->setMat4("model",  glm::mat4(1.0f));
    blinnphong->setMat4("view_proj", view_proj);
    blinnphong->setMat4("light_view_proj", light_view_proj);
    blinnphong->setVec3("camera_position", camera.position);
        
    blinnphong->setFloat("ambient.intensity", ambient.intensity);
    blinnphong->setVec3("ambient.color", ambient.color);

    blinnphong->setVec3("light.position", light.position);
    blinnphong->setVec3("light.color", light.color);

    blinnphong->setFloat("bias", settings.bias);
    blinnphong->setFloat("use_pcf", settings.use_pcf);

    // Draw
    blinnphong->setMat4("model", glm::translate(glm::vec3(0.0f, 20.0f, 0.0f)));
    skull->draw();

    blinnphong->setMat4("model", glm::translate(glm::vec3(0.0f, -20.0f, 0.0f)));
    planeMesh.draw();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // // Smaller fullscrean window
    // {
    //     depth->setInt("screen", 0);
        
    //     // Fullscreen pipeline 
    //     glDisable(GL_DEPTH_TEST);

    //     // default frame buffer
    //     glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //     // draw fullscreenquad
    //     glBindVertexArray(fullscreenQuad.vao);
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, frameBuffer.color0);
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
    /* build debug ui here */

    ImGui::SeparatorText("Shadow Mapping");
    ImGui::Checkbox("cull_front", &settings.cull_front);
    ImGui::Checkbox("use_pcf", &settings.use_pcf);
    ImGui::SliderFloat("Bias", &settings.bias, 0.05f, 0.00f);

    ImGui::SeparatorText("Material");
    ImGui::SliderFloat3("Ambient", &settings.material.ambient[0], 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", &settings.material.diffuse[0], 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular", &settings.material.specular[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &settings.material.shininess, 0.0f, 1.0f);

    ImGui::SeparatorText("Ambient");
    ImGui::SliderFloat("Intensity", &ambient.intensity, 0.0f, 1.0f);
    ImGui::ColorEdit3("Color", &ambient.color[0]);

    ImGui::Text("Depth:");
    ImGui::Image(
        (ImTextureID)(intptr_t)depthBuffer.depth, 
        ImVec2(400, 300),
        ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}