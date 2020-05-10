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

Camera camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));

void processInput(int key, int scancode, int action, int mods);
void mouse_callback(double xpos, double ypos);
void scroll_callback(double xoffset, double yoffset);
void mouse_button_callback(int button, int action, int mods);
void framebuffer_size_callback(int width, int height);

estun::WindowConfig winConf = {"Water Simulation", WIDTH, HEIGHT, "assets/textures/icon.png", false, false, true};
estun::GameInfo info("test", {0, 0, 1}, WIDTH, HEIGHT, true, true);
std::unique_ptr<estun::Window> window;

struct LightUBO
{
    glm::vec4 lightPos;
    glm::vec4 lightColor;
};

struct VertexUBO
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 underView;
    glm::mat4 projection;
    glm::vec4 cameraPos;
};

struct ComputeUBO
{
    glm::vec4 mouse;
    float scale;
};
ComputeUBO compUBO = {};

struct VertexPC
{
    glm::vec4 clipPlane;
    float reflect;
};

int surface_size = 256;
float surface_scale = 10.0f;
bool wireframe = false;

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

    VertexUBO ubo = {};

    LightUBO light = {};
    light.lightColor = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f);
    light.lightPos = glm::vec4(0.0F, 2.0F, 0.0F, 1.0f);

    compUBO.scale = surface_size;

    VertexPC vertexPc = {};

    std::vector<estun::UniformBuffer<VertexUBO>> UBs(context->GetSwapChain()->GetImageViews().size());
    std::vector<estun::UniformBuffer<LightUBO>> fragUBs(context->GetSwapChain()->GetImageViews().size());
    std::vector<estun::UniformBuffer<ComputeUBO>> compUBs(context->GetSwapChain()->GetImageViews().size());
    std::vector<estun::Model> models;
    models.push_back(estun::Model::LoadModel("terrain", "assets/models/terrain.obj"));
    estun::Material material = estun::Material::Lambertian(glm::vec3(0.5f, 0.5f, 0.5f), 0);
    models[0].SetMaterial(material);
    models[0].Transform(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(6.0f)), glm::vec3(0.0f, -0.06f, 0.0f)));

    models.push_back(WaterSurface::CreateSurface(surface_scale, surface_size, surface_size));
    estun::Material colorMaterial = estun::Material::Lambertian(glm::vec3(0.5f, 0.5f, 0.5f), -1);

    models.push_back(estun::Model::CreateBox(glm::vec3(0.0f), glm::vec3(0.5f), colorMaterial));
    models.back().Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -2.0f)));
    models.push_back(estun::Model::CreateBox(glm::vec3(0.0f), glm::vec3(0.5f), colorMaterial));
    models.back().Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -0.2f, 0.0f)));
    models.push_back(estun::Model::CreateBox(glm::vec3(0.0f), glm::vec3(0.5f), colorMaterial));
    models.back().Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -0.2f, -2.0f)));

    models.push_back(estun::Model::CreateSphere(glm::vec3(0.0f), 0.5f, colorMaterial));
    models.back().Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)));
    models.push_back(estun::Model::CreateSphere(glm::vec3(0.0f), 0.5f, colorMaterial));
    models.back().Transform(glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -0.5f, -2.0f)));
    models.push_back(estun::Model::CreateSphere(glm::vec3(0.0f), 0.5f, colorMaterial));
    models.back().Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -2.0f)));

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

    std::shared_ptr<estun::Render> reflect = context->CreateRender(false);
    std::shared_ptr<estun::Render> refract = context->CreateRender(false);
    std::shared_ptr<estun::Render> allRender = context->CreateRender();

    std::shared_ptr<estun::ComputeRender> compRender = context->CreateComputeRender();

    std::vector<estun::DescriptorBinding> descriptorBindings = {
        estun::DescriptorBinding::Uniform(0, UBs, VK_SHADER_STAGE_VERTEX_BIT),
        estun::DescriptorBinding::Storage(1, materialBuffer, VK_SHADER_STAGE_FRAGMENT_BIT),
        estun::DescriptorBinding::Uniform(2, fragUBs, VK_SHADER_STAGE_FRAGMENT_BIT),
        estun::DescriptorBinding::Textures(3, textures, VK_SHADER_STAGE_FRAGMENT_BIT)};

    estun::PushConstant<VertexPC> pc = estun::PushConstant<VertexPC>(VK_SHADER_STAGE_VERTEX_BIT);

    std::shared_ptr<estun::Descriptor> descriptor = std::make_shared<estun::Descriptor>(descriptorBindings, context->GetSwapChain()->GetImageViews().size());
    descriptor->AddPushConstants(pc);
    descriptorBindings.clear();

    std::vector<std::shared_ptr<estun::ImageHolder>> compImages;
    for (int i = 0; i < 3; i++)
    {
        compImages.push_back(std::make_shared<estun::ImageHolder>(
            surface_size, surface_size,
            VK_SAMPLE_COUNT_1_BIT,
            VK_FORMAT_R16G16B16A16_SFLOAT,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT));
        compImages.back()->ToLayout(VK_IMAGE_LAYOUT_GENERAL);
    }

    std::vector<estun::DescriptorBinding> compDB = {
        estun::DescriptorBinding::StorageImage(0, compImages[0], VK_SHADER_STAGE_COMPUTE_BIT),
        estun::DescriptorBinding::StorageImage(1, compImages[1], VK_SHADER_STAGE_COMPUTE_BIT),
        estun::DescriptorBinding::StorageImage(2, compImages[2], VK_SHADER_STAGE_COMPUTE_BIT),
        estun::DescriptorBinding::Uniform(3, compUBs, VK_SHADER_STAGE_COMPUTE_BIT)};

    std::shared_ptr<estun::Descriptor> compDescriptor = std::make_shared<estun::Descriptor>(compDB, context->GetSwapChain()->GetImageViews().size());
    compDB.clear();

    std::vector<estun::DescriptorBinding> waterDB = {
        estun::DescriptorBinding::Uniform(0, UBs, VK_SHADER_STAGE_VERTEX_BIT),
        estun::DescriptorBinding::ImageSampler(1, reflect->GetColorResources(), VK_SHADER_STAGE_FRAGMENT_BIT),
        estun::DescriptorBinding::ImageSampler(2, refract->GetColorResources(), VK_SHADER_STAGE_FRAGMENT_BIT),
        estun::DescriptorBinding::ImageSampler(3, compImages[2], VK_SHADER_STAGE_VERTEX_BIT)};

    std::shared_ptr<estun::Descriptor> waterDescriptor = std::make_shared<estun::Descriptor>(waterDB, context->GetSwapChain()->GetImageViews().size());
    waterDB.clear();

    std::shared_ptr<estun::GraphicsPipeline> refractPipeline = refract->CreatePipeline(
        {{"assets/shaders/main.vert.spv", VK_SHADER_STAGE_VERTEX_BIT},
         {"assets/shaders/main.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT}},
        descriptor);
    std::shared_ptr<estun::GraphicsPipeline> reflectPipeline = reflect->CreatePipeline(
        {{"assets/shaders/main.vert.spv", VK_SHADER_STAGE_VERTEX_BIT},
         {"assets/shaders/main.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT}},
        descriptor);

    std::shared_ptr<estun::GraphicsPipeline> waterPipeline = allRender->CreatePipeline(
        {{"assets/shaders/water.vert.spv", VK_SHADER_STAGE_VERTEX_BIT},
         {"assets/shaders/water.geom.spv", VK_SHADER_STAGE_GEOMETRY_BIT},
         {"assets/shaders/water.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT}},
        waterDescriptor);
    std::shared_ptr<estun::GraphicsPipeline> waterWireframePipeline = allRender->CreatePipeline(
        {{"assets/shaders/water.vert.spv", VK_SHADER_STAGE_VERTEX_BIT},
         {"assets/shaders/water.geom.spv", VK_SHADER_STAGE_GEOMETRY_BIT},
         {"assets/shaders/water.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT}},
        waterDescriptor, true);
    std::shared_ptr<estun::GraphicsPipeline> terrainPipeline = allRender->CreatePipeline(
        {{"assets/shaders/main.vert.spv", VK_SHADER_STAGE_VERTEX_BIT},
         {"assets/shaders/main.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT}},
        descriptor);

    std::shared_ptr<estun::ComputePipeline> compPipeline = compRender->CreatePipeline("assets/shaders/surface.comp.spv", compDescriptor);
    int frame_count = 0;
    while (!glfwWindowShouldClose(window->GetWindow()))
    {
        float currFrame = window->Time();
        deltaTime = lastFrame - currFrame;
        lastFrame = currFrame;
        glfwPollEvents();

        context->StartDraw();

        ubo.view = camera.GetViewMatrix();
        ubo.underView = camera.GetUnderViewMatrix();
        ubo.projection = glm::perspective(glm::radians(camera.Zoom), (float)info.width_ / (float)info.height_, 0.01f, 1000.0f);
        ubo.cameraPos = glm::vec4(camera.GetPosition(), 1.0f);
        vertexPc.reflect = 1;
        vertexPc.clipPlane = glm::vec4(0, 1, 0, 0);

        for (auto &render : {reflect, refract})
        {
            pc.SetConst(vertexPc);
            render->StartDrawInCurrent();
            render->Bind(pc, descriptor);
            render->Bind(descriptor);
            render->Bind(render->GetPipelines()[0]);
            render->Bind(VB);
            render->Bind(IB);

            uint32_t vertexOffset = 0;
            uint32_t indexOffset = 0;
            for (const auto &model : models)
            {
                const auto vertexCount = static_cast<uint32_t>(model.GetVertices().size());
                const auto indexCount = static_cast<uint32_t>(model.GetIndices().size());
                if (model.GetName() != "surface")
                {
                    render->DrawIndexed(indexCount, indexOffset, vertexOffset);
                }
                vertexOffset += vertexCount;
                indexOffset += indexCount;
            }

            render->RecordDrawInCurrent();

            vertexPc.clipPlane = glm::vec4(0, -1, 0, 0);
            vertexPc.reflect = 0;
        }

        allRender->StartDrawInCurrent();
        allRender->Bind(VB);
        allRender->Bind(IB);

        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;

        vertexPc.reflect = 0;
        vertexPc.clipPlane = glm::vec4(0, 0, 0, 0);
        pc.SetConst(vertexPc);
        allRender->Bind(pc, descriptor);
        for (const auto &model : models)
        {
            const auto vertexCount = static_cast<uint32_t>(model.GetVertices().size());
            const auto indexCount = static_cast<uint32_t>(model.GetIndices().size());
            if (model.GetName() == "surface")
            {
                allRender->Bind(waterDescriptor);
                if (wireframe)
                {
                    allRender->Bind(waterWireframePipeline);
                }
                else
                {
                    allRender->Bind(waterPipeline);
                }
            }
            else
            {
                allRender->Bind(descriptor);
                allRender->Bind(terrainPipeline);
            }
            allRender->DrawIndexed(indexCount, indexOffset, vertexOffset);
            vertexOffset += vertexCount;
            indexOffset += indexCount;
        }

        allRender->RecordDrawInCurrent();

        ubo.model = glm::mat4(1.0f);
        UBs[context->GetImageIndex()].SetValue(ubo);
        fragUBs[context->GetImageIndex()].SetValue(light);
        compUBs[context->GetImageIndex()].SetValue(compUBO);

        compRender->Start();
        compRender->ComputeMemoryBarrier();
        compRender->Bind(compDescriptor);
        compRender->Bind(compPipeline);
        compRender->Dispath(surface_size, surface_size);
        compRender->ImageBarrier(
            compImages[1], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        compRender->ImageBarrier(
            compImages[0], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        compRender->CopyImage(compImages[1], compImages[0]);
        compRender->ImageBarrier(
            compImages[0], VK_IMAGE_LAYOUT_GENERAL,
            VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        compRender->ImageBarrier(
            compImages[1], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        compRender->ImageBarrier(
            compImages[2], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        compRender->CopyImage(compImages[2], compImages[1]);
        compRender->ImageBarrier(
            compImages[1], VK_IMAGE_LAYOUT_GENERAL,
            VK_ACCESS_SHADER_READ_BIT, 0,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        compRender->ImageBarrier(
            compImages[2], VK_IMAGE_LAYOUT_GENERAL,
            VK_ACCESS_SHADER_READ_BIT, 0,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        compRender->End();

        frame_count = (frame_count + 1) % 10;

        context->SubmitDraw();
    }

    allRender.reset();
    reflect.reset();
    refract.reset();
    compRender.reset();
    terrainPipeline.reset();
    waterPipeline.reset();
    waterWireframePipeline.reset();
    reflectPipeline.reset();
    refractPipeline.reset();
    compPipeline.reset();
    context->Clear();
    UBs.clear();
    fragUBs.clear();
    compUBs.clear();
    compImages.clear();
    VB.reset();
    IB.reset();
    textures.clear();
    materialBuffer.reset();
    descriptor.reset();
    waterDescriptor.reset();
    compDescriptor.reset();
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
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        wireframe = !wireframe;
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
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        compUBO.mouse.z = 1.0f;
        glm::vec2 pos;
        glm::vec3 camPos = camera.GetPosition();
        glm::vec3 camFront = camera.Front;
        if (glm::abs(camFront.y) > 0.00001f)
        {
            float t = (-camPos.y) / camFront.y;
            pos.x = camPos.x + camFront.x * t;
            pos.y = camPos.z + camFront.z * t;

            ES_CORE_INFO(compUBO.mouse.x);
            ES_CORE_INFO(compUBO.mouse.y);
            if ((glm::abs(pos.x)) < surface_scale && (glm::abs(pos.y) < surface_scale))
            {
                compUBO.mouse.x = (pos.x + surface_scale) / (2.0f * surface_scale);
                compUBO.mouse.y = (pos.y + surface_scale) / (2.0f * surface_scale);
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        compUBO.mouse = glm::vec4(0.0f);
        ES_CORE_INFO("0");
    }
}

void framebuffer_size_callback(int width, int height)
{
    info.width_ = width;
    info.height_ = height;
}
