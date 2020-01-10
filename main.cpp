#include "renderer/vulkan/vulkan_context.h"
#include "renderer/vulkan/vulkan_material.h"
#include "renderer/vulkan/material/vulkan_material_pool.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include "tiny_obj_loader.h"
#include "core/camera.h"

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
estun::VulkanContext* context;

estun::GameInfo info("test", 1, WIDTH, HEIGHT);

void LoadModel(std::string path, std::vector<estun::Vertex> &vertices, std::vector<uint32_t> &indices);
void CreateSurface(std::vector<estun::Vertex> &vertices, std::vector<uint32_t> &indices);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool put = false;

int main(int argc, const char **argv)
{
    estun::Log::Init();

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    context = new estun::VulkanContext(window, &info);
    estun::VulkanContextLocator::Provide(context);
    estun::VulkanMaterialPool* materialPool = new estun::VulkanMaterialPool();
    estun::VulkanMaterialPoolLocator::Provide(materialPool);

    std::vector<estun::Vertex> vertices = {};
    std::vector<uint32_t> indices = {};
    LoadModel("assets/models/cube.obj", vertices, indices);

    estun::VulkanMaterial* material = new estun::VulkanMaterial();
    estun::VulkanUniformBuffer* ub = new estun::VulkanUniformBuffer();
    //estun::VulkanVertexBuffer* vb = new estun::VulkanVertexBuffer(&vertices);
    //estun::VulkanIndexBuffer* ib = new estun::VulkanIndexBuffer(&indices);

    material->CreatePipeline("assets/shaders/main.vert.spv", "assets/shaders/main.frag.spv");
    material->CreateTexture("assets/textures/box.jpg");
    material->CreateDescriptorSets(ub);


    std::vector<estun::Vertex> surfaceVertices = {};
    std::vector<uint32_t> surfaceIndices = {};
    CreateSurface(surfaceVertices, surfaceIndices);

    estun::VulkanMaterial* surfaceMaterial = new estun::VulkanMaterial(true);
    estun::VulkanUniformBuffer* surfaceUB = new estun::VulkanUniformBuffer();
    std::vector<estun::VulkanVertexBuffer*> surfaceVBs ={ 
        new estun::VulkanVertexBuffer(&surfaceVertices),
        new estun::VulkanVertexBuffer(&surfaceVertices),
        new estun::VulkanVertexBuffer(&surfaceVertices)
    };
    estun::VulkanVertexBuffer* vb = new estun::VulkanVertexBuffer(&surfaceVertices);
    estun::VulkanIndexBuffer* surfaceIB = new estun::VulkanIndexBuffer(&surfaceIndices);

    surfaceMaterial->CreatePipeline("assets/shaders/surface.vert.spv", "assets/shaders/surface.frag.spv");
    surfaceMaterial->CreateComputePipeline("assets/shaders/surface.comp.spv");
    surfaceMaterial->CreateTexture("assets/textures/box.jpg");
    surfaceMaterial->CreateComputeDescriptorSets(surfaceVBs[1], surfaceVBs[2], surfaceVBs[0]);
    int currVB = 0;
    surfaceMaterial->CreateDescriptorSets(ub);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        context->PrepareFrame();
        surfaceMaterial->Update(surfaceVBs[(currVB + 1) % 3], surfaceVBs[(currVB + 2) % 3], surfaceVBs[currVB]);
        context->SubmitCompute(GRID_SIZE_X, GRID_SIZE_Y, surfaceMaterial);
        context->StartCompute();

        //context->Submit(vb, surfaceIB, material);
        context->Submit(surfaceVBs[currVB], surfaceIB, surfaceMaterial);

        estun::UniformBufferObject newubo = {};
        newubo.view = camera.GetViewMatrix();
        newubo.model = glm::mat4(1.0f);
        newubo.model = glm::translate(newubo.model, {0.0, -1.0, 0.0});
        newubo.proj = projection;
        ub->Update(newubo);
        newubo.model = glm::mat4(1.0f);
        surfaceUB->Update(newubo);

        context->SubmitFrame();

        processInput(window);

        glfwPollEvents();
        currVB = (currVB + 1) % 3;
    }
    context->EndDraw();

    glfwDestroyWindow(window);
    context->FreeComandBuffers();
    glfwTerminate();
    delete surfaceIB;
    for ( auto surfaceVB : surfaceVBs)
    {
        delete surfaceVB;
    }
    surfaceVBs.clear();
    delete surfaceUB;
    delete surfaceMaterial;
    //delete ib;
    delete vb;
    delete ub;
    delete material;
    materialPool->CleanUp();
    delete materialPool;
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    info.width = width;
    info.height = height;
}

void LoadModel(std::string path, std::vector<estun::Vertex> &vertices, std::vector<uint32_t> &indices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<estun::Vertex, uint32_t> uniqueVertices = {};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            estun::Vertex vertex = {};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 2],
                attrib.vertices[3 * index.vertex_index + 1], 1.0f
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1], 0.0f, 0.0f
            };

            vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

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