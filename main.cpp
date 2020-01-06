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

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
estun::VulkanContext* context;

estun::GameInfo info("test", 1, WIDTH, HEIGHT);

void LoadModel(std::string path, std::vector<estun::Vertex> &vertices, std::vector<uint32_t> &indices);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

int main(int argc, const char **argv)
{
    estun::Log::Init();

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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
    estun::VulkanVertexBuffer* vb = new estun::VulkanVertexBuffer(&vertices);
    estun::VulkanIndexBuffer* ib = new estun::VulkanIndexBuffer(&indices);

    material->CreatePipeline("assets/shaders/main.vert.spv", "assets/shaders/main.frag.spv");
    material->CreateTexture("assets/textures/box.jpg");
    estun::UniformBufferObject ubo = {};
    material->CreateDescriptorSets(ub);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        estun::UniformBufferObject newubo = {};
        newubo.view = camera.GetViewMatrix();
        newubo.model = glm::mat4(1.0f);
        newubo.proj = projection;
        context->Submit(vb, ib, material);
        context->PrepareFrame();
        ub->Update(newubo);
        context->SubmitFrame();

        processInput(window);

        glfwPollEvents();
    }
    context->EndDraw();

    glfwDestroyWindow(window);
    context->FreeComandBuffers();
    glfwTerminate();
    delete ib;
    delete vb;
    delete ub;
    delete material;
    materialPool->CleanUp();
    delete materialPool;
    delete context;
    return 0;
}

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
    context->ResizeFramebuffers();
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
                attrib.vertices[3 * index.vertex_index + 1]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}