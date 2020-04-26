#include "estun.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include "tiny_obj_loader.h"

#include "CornellBox.h"

#define WIDTH 600
#define HEIGHT 400

#define GRID_SIZE_X 101
#define GRID_SIZE_Y 101

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
estun::Context *context;

estun::GameInfo info("test", {0, 0, 1}, WIDTH, HEIGHT, true, false);

//void CreateSurface(std::vector<estun::Vertex> &vertices, std::vector<uint32_t> &indices);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool put = false;

int main(int argc, const char **argv)
{
    estun::Log::Init();

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    context = new estun::Context(window, &info);
    estun::ContextLocator::Provide(context);
    estun::Model cornell = CornellBox::CreateCornellBox(1.0);

    //const auto cameraRotX = static_cast<float>(cameraY_ / 300.0);
    //const auto cameraRotY = static_cast<float>(cameraX_ / 300.0);

    //const auto& init = cameraInitialSate_;
    const auto view = glm::mat4(1.0f); //init.ModelView;
    const auto model = glm::mat4(1.0f);

    unsigned maxNumberOfSamples = 1024;
    unsigned numberOfBounces = 4;
    unsigned numberOfSamples = 8;
    unsigned totalNumberOfSamples = 0;

    estun::UniformBufferObject ubo = {};
    ubo.ModelView = view * model;
    ubo.Projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 10000.0f);
    ubo.Projection[1][1] *= -1;
    ubo.ModelViewInverse = glm::inverse(ubo.ModelView);
    ubo.ProjectionInverse = glm::inverse(ubo.Projection);
    ubo.Aperture = 0.0f;
    ubo.FocusDistance = 10.0f;
    ubo.TotalNumberOfSamples = totalNumberOfSamples; //сколько всего накопил
    ubo.NumberOfSamples = numberOfSamples;           // сколько накопил с ограничением
    ubo.NumberOfBounces = numberOfBounces;
    ubo.RandomSeed = 1;
    ubo.GammaCorrection = true;
    ubo.HasSky = false;

    bool resetAccumulation = false;

    std::vector<estun::UniformBuffer> UBs(context->GetSwapChain()->GetImageViews().size());
    std::vector<glm::uvec2> offsets;
    const auto indexOffset = static_cast<uint32_t>(0);    // indices.size()
    const auto vertexOffset = static_cast<uint32_t>(0);   // vertices.size()
    const auto materialOffset = static_cast<uint32_t>(0); // materials.size()
    /*
        for (size_t i = vertexOffset; i != vertices.size(); ++i)
		{
			vertices[i].MaterialIndex += materialOffset;
		}
    */
    offsets.emplace_back(indexOffset, vertexOffset);
    estun::VertexBuffer VB(cornell.GetVertices());
    estun::IndexBuffer IB(cornell.GetIndices());
    //estun::AccelerationStructureManager ASM;
    std::vector<estun::Model> models = {cornell};
    //ASM.Submit(models, &VB, &IB, false);

    //estun::StorageBuffer materialBuffer(cornell.GetMaterials());
    //estun::StorageBuffer offsetBuffer(offsets);

    std::vector<estun::Texture> textures;

    std::vector<estun::DescriptorBinding> descriptorBindings =
        {
            //estun::DescriptorBinding::AccelerationStructure(0, ASM.GetTLAS()[0], VK_SHADER_STAGE_RAYGEN_BIT_NV),
            //estun::DescriptorBinding::ImageHolder(1, *context->GetAccumulationImage(), VK_SHADER_STAGE_RAYGEN_BIT_NV),
            //estun::DescriptorBinding::ImageHolder(2, *context->GetOutputImage(), VK_SHADER_STAGE_RAYGEN_BIT_NV),
            estun::DescriptorBinding::Uniform(3, UBs, VK_SHADER_STAGE_RAYGEN_BIT_NV | VK_SHADER_STAGE_MISS_BIT_NV),
            //estun::DescriptorBinding::Storage(4, VB, VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV),
            //estun::DescriptorBinding::Storage(5, IB, VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV),
            //estun::DescriptorBinding::Storage(6, materialBuffer, VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV),
            //estun::DescriptorBinding::Storage(7, offsetBuffer, VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV),
            //estun::DescriptorBinding::Textures(8, textures, VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV),
        };

    estun::Descriptor descriptor(descriptorBindings, context->GetSwapChain()->GetImageViews().size());
    /*
    std::vector<std::string> RTShaders = {
        "../assets/shaders/RayTracing.rgen.spv",
        "../assets/shaders/RayTracing.rmiss.spv",
        "../assets/shaders/RayTracing.rchit.spv",
    };
    estun::RayTracingPipeline pipeline(RTShaders, ASM.GetTLAS()[0], context->GetAccumulationImage()->GetImageView(), context->GetOutputImage()->GetImageView(), descriptor, UBs);
    estun::RayTracingProperties properties;

    const std::vector<estun::ShaderBindingTable::Entry> rayGenPrograms = {{pipeline.RayGenShaderIndex(), {}}};
    const std::vector<estun::ShaderBindingTable::Entry> missPrograms = {{pipeline.MissShaderIndex(), {}}};
    const std::vector<estun::ShaderBindingTable::Entry> hitGroups = {{pipeline.TriangleHitGroupIndex(), {}}};

    estun::ShaderBindingTable shaderBindingTable(pipeline, properties, rayGenPrograms, missPrograms, hitGroups);
*/
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    estun::Render render;

    while (!glfwWindowShouldClose(window))
    {
        if (resetAccumulation)
        {
            totalNumberOfSamples = 0;
            resetAccumulation = false;
        }
        numberOfSamples = glm::clamp(maxNumberOfSamples - totalNumberOfSamples, 0u, numberOfSamples);
        totalNumberOfSamples += numberOfSamples;

        context->StartDraw();
        render.StartDrawInCurrent();
        //context->StartRayTracing();
       // pipeline.Bind();
       // descriptor.Bind();
        //context->EndRayTracing(shaderBindingTable);
        //context->EndDraw();
        render.RecordDrawInCurrent();
        std::vector<estun::Render> renders;
        renders.push_back(render);
        // UBs[context->GetImageIndex()].SetValue(ubo);
        context->SubmitDraw(renders);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    delete context;
    return 0;
}

bool cameraview = false;

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        if (cameraview)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        cameraview = !cameraview;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        put = true;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    info.width_ = width;
    info.height_ = height;
}
/*
float GetY(int y)
{
    return ((float)y - GRID_SIZE_Y / 2) / GRID_SIZE_Y;
}

float GetX(int x)
{
    return ((float)x - GRID_SIZE_X / 2) / GRID_SIZE_X;
}

void CreateSurface(std::vector<estun::Vertex> &vertices, std::vector<uint32_t> &indices)
{

    std::unordered_map<estun::Vertex, uint32_t> uniqueVertices = {};

    for (int i = 0; i < GRID_SIZE_Y; i++) 
    {
        for (int j = 0; j < GRID_SIZE_X; j++) 
        {
            estun::Vertex vertex = {};
            if(i == GRID_SIZE_Y/2 && j == GRID_SIZE_X/2){
                vertex.pos = { GetX(i), 1.0f, GetY(j), 1.0f};
            }
            else
            {
                vertex.pos = { GetX(i), 0.0f, GetY(j), 1.0f};
            }
            
            vertex.texCoord = { 0.0f, 0.0f, 0.0f, 0.0f };
            vertex.color = {0.0f, 0.0f, 1.0f, 1.0f};
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
        }
    }

    for (int i = 0; i < GRID_SIZE_Y - 1; i++) 
    {
        for (int j = 0; j < GRID_SIZE_X - 1; j++) 
        {
            indices.push_back(i * GRID_SIZE_X + j);
            indices.push_back(i * GRID_SIZE_X + j + 1);
            indices.push_back((i + 1) * GRID_SIZE_X + j + 1);


            indices.push_back((i + 1) * GRID_SIZE_X + j + 1);
            indices.push_back((i + 1) * GRID_SIZE_X + j);
            indices.push_back(i * GRID_SIZE_X + j);
        }
    }
}
*/