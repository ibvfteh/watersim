#include "renderer/vulkan/vulkan_context.h"
#include "renderer/vulkan/vulkan_material.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include "tiny_obj_loader.h"

#define WIDTH 600
#define HEIGHT 400


void LoadModel(std::string path, std::vector<estun::Vertex> &vertices, std::vector<uint32_t> &indices);

int main(int argc, const char **argv)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    estun::Log::Init();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    estun::GameInfo info("test", 1, WIDTH, HEIGHT);
    estun::VulkanContext* context = new estun::VulkanContext(window, &info);

    std::vector<estun::Vertex> vertices = {};
    std::vector<uint32_t> indices = {};
    LoadModel("assets/models/cube.obj", vertices, indices);

    estun::VulkanMaterial* material;
    estun::VulkanUniformBuffer* ub = new estun::VulkanUniformBuffer();
    estun::VulkanVertexBuffer* vb = new estun::VulkanVertexBuffer(&vertices);
    estun::VulkanIndexBuffer* ib = new estun::VulkanIndexBuffer(&indices);
    material->CreatePipeline("assets/shaders/main.vert", "assets/sahderes/main.frag");
    material->CreateTexture("assets/textures/box.jpg");
    estun::UniformBufferObject ubo = {};
    material->CreateDescriptorSets(ub);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    delete context;
    return 0;
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
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
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