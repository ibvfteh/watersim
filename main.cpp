#include "estun.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include "tiny_obj_loader.h"

#include "cornell_box.h"
#include "water_surface.h"

#define WIDTH 600
#define HEIGHT 400

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 2.5f, 5.0f));

void processInput(int key, int scancode, int action, int mods);
void mouse_callback(double xpos, double ypos);
void scroll_callback(double xoffset, double yoffset);
void mouse_button_callback(int button, int action, int mods);
void framebuffer_size_callback(int width, int height);

estun::WindowConfig winConf = {"Water Simulation", WIDTH, HEIGHT, "assets/textures/icon.png", false, false, true};
estun::GameInfo info("test", {0, 0, 1}, WIDTH, HEIGHT, true, true);
std::unique_ptr<estun::Window> window;

struct Light
{
    glm::vec4 lightPos;
    glm::vec4 lightColor;
};

int main(int argc, const char **argv)
{
    estun::Log::Init();
    window = std::make_unique<estun::Window>(winConf);
    window->OnKey = processInput;
    window->OnCursorPosition = mouse_callback;
    window->OnScroll = scroll_callback;
    window->OnMouseButton = mouse_button_callback;
    window->OnResize = framebuffer_size_callback;
    window->ToggleCursor(false);

    std::shared_ptr<estun::Context> context = std::make_shared<estun::Context>(window->GetWindow(), &info);
    estun::ContextLocator::Provide(context.get());

    estun::UniformBufferObject ubo = {};

    Light light = {};
    light.lightColor = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f);
    light.lightPos = glm::vec4(0.0F, 2.0F, 0.0F, 1.0f);

    std::vector<estun::UniformBuffer> UBs(context->GetSwapChain()->GetImageViews().size());
    std::vector<estun::UniformBuffer> fragUBs(context->GetSwapChain()->GetImageViews().size());
    std::vector<estun::Model> models;
    models.push_back(estun::Model::LoadModel("assets/models/terrain.obj"));
    estun::Material material = estun::Material::Lambertian(glm::vec3(1.0f, 0.0f, 1.0f), 0);
    models[0].SetMaterial(material);

    models.push_back(WaterSurface::CreateSurface(2.5f, 100, 100));
    models[1].Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.06f, 0.0f)));
    //models.push_back(CornellBox::CreateCornellBox(1.0f));

    //estun::Model model(CornellBox::CreateCornellBox(1.0f));

    std::vector<estun::Texture> textures;
    textures.push_back(estun::Texture("assets/textures/terrain.png"));

    std::vector<estun::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<estun::Material> materials;

    for (const auto &model : models)
    {
        const auto vertexOffset = static_cast<uint32_t>(vertices.size());
        const auto materialOffset = static_cast<uint32_t>(materials.size());

        vertices.insert(vertices.end(), model.GetVertices().begin(), model.GetVertices().end());
        indices.insert(indices.end(), model.GetIndices().begin(), model.GetIndices().end());
        materials.insert(materials.end(), model.GetMaterials().begin(), model.GetMaterials().end());

        for (size_t i = vertexOffset; i != vertices.size(); ++i)
        {
            vertices[i].materialIndex += materialOffset;
        }
    }

    std::shared_ptr<estun::VertexBuffer> VB = std::make_shared<estun::VertexBuffer>(vertices);
    std::shared_ptr<estun::IndexBuffer> IB = std::make_shared<estun::IndexBuffer>(indices);
    std::shared_ptr<estun::StorageBuffer<estun::Material>> materialBuffer = std::make_shared<estun::StorageBuffer<estun::Material>>(materials);

    std::vector<estun::DescriptorBinding> descriptorBindings = {
        estun::DescriptorBinding::Uniform(0, UBs, VK_SHADER_STAGE_VERTEX_BIT),
        estun::DescriptorBinding::Storage(1, materialBuffer, VK_SHADER_STAGE_FRAGMENT_BIT),
        estun::DescriptorBinding::Uniform(2, fragUBs, VK_SHADER_STAGE_FRAGMENT_BIT),
        estun::DescriptorBinding::Textures(3, textures, VK_SHADER_STAGE_FRAGMENT_BIT)};

    std::shared_ptr<estun::Descriptor> descriptor = std::make_shared<estun::Descriptor>(descriptorBindings, context->GetSwapChain()->GetImageViews().size());
    descriptorBindings.clear();
    std::shared_ptr<estun::Render> render = context->CreateRender();
    std::shared_ptr<estun::GraphicsPipeline> pipeline = render->CreatePipeline("shaders/main.vert.spv", "shaders/main.frag.spv", descriptor);
    
    std::shared_ptr<estun::Render> reflect = context->CreateRender(false);
    std::shared_ptr<estun::Render> refract = context->CreateRender(false);

    while (!glfwWindowShouldClose(window->GetWindow()))
    {
        float currFrame = window->Time();
        deltaTime = lastFrame - currFrame;
        lastFrame = currFrame;
        glfwPollEvents();

        context->StartDraw();

        ubo.view = camera.GetViewMatrix();
        ubo.projection = glm::perspective(glm::radians(camera.Zoom), (float)info.width_ / (float)info.height_, 0.1f, 10000.0f);

        render->StartDrawInCurrent();
        render->Bind(descriptor);
        render->Bind(pipeline);
        render->Bind(VB);
        render->Bind(IB);

        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;
        for (const auto &model : models)
        {
            ubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));
            UBs[context->GetImageIndex()].SetValue<estun::UniformBufferObject>(ubo);

            const auto vertexCount = static_cast<uint32_t>(model.GetVertices().size());
            const auto indexCount = static_cast<uint32_t>(model.GetIndices().size());
            render->DrawIndexed(indexCount, indexOffset, vertexOffset);
            vertexOffset += vertexCount;
            indexOffset += indexCount;
        }

        render->RecordDrawInCurrent();

        ubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
        UBs[context->GetImageIndex()].SetValue<estun::UniformBufferObject>(ubo);
        fragUBs[context->GetImageIndex()].SetValue<Light>(light);

        context->SubmitDraw();
    }

    render.reset();
    reflect.reset();
    refract.reset();
    pipeline.reset();
    context->Clear();
    UBs.clear();
    fragUBs.clear();
    VB.reset();
    IB.reset();
    textures.clear();
    materialBuffer.reset();
    descriptor.reset();
    window.reset();
    context.reset();
    return 0;
}

bool cursor = false;

void processInput(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        window->Close();
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT) && !cursor)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT) && !cursor)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT) && !cursor)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT) && !cursor)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        window->ToggleCursor(!cursor);
        cursor = !cursor;
        ES_CORE_INFO(cursor);
    }
}

void mouse_callback(double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void mouse_button_callback(int button, int action, int mods)
{
}

void framebuffer_size_callback(int width, int height)
{
    info.width_ = width;
    info.height_ = height;
}
