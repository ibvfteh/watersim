#include "estun.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include "tiny_obj_loader.h"

#include "cornell_box.h"

#define WIDTH 600
#define HEIGHT 400

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

estun::GameInfo info("test", {0, 0, 1}, WIDTH, HEIGHT, true, false);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool put = false;

int main(int argc, const char **argv)
{

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    estun::Log::Init();

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE)

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    std::shared_ptr<estun::Context> context = std::make_shared<estun::Context>(window, &info);
    estun::ContextLocator::Provide(context.get());

    estun::UniformBufferObject ubo = {};
    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 10.0f);
    //ubo.projection[1][1] *= -1;

    std::vector<estun::UniformBuffer> UBs(context->GetSwapChain()->GetImageViews().size());
    estun::Model model(CornellBox::CreateCornellBox(1.0f));

    std::shared_ptr<estun::VertexBuffer> VB = std::make_shared<estun::VertexBuffer>(model.GetVertices());
    std::shared_ptr<estun::IndexBuffer> IB = std::make_shared<estun::IndexBuffer>(model.GetIndices());
    //std::shared_ptr<estun::AccelerationStructureManager> ASM = std::make_shared<estun::AccelerationStructureManager>();
    //ASM->Submit(std::vector<estun::Model>{model}, VB.get(), IB.get(), false);

    std::vector<estun::DescriptorBinding> descriptorBindings = {
        estun::DescriptorBinding::Uniform(0, UBs, VK_SHADER_STAGE_VERTEX_BIT)
    };

    std::shared_ptr<estun::Descriptor> descriptor = std::make_shared<estun::Descriptor>(descriptorBindings, context->GetSwapChain()->GetImageViews().size());
    descriptorBindings.clear();
    std::shared_ptr<estun::Render> render = context->CreateRender();
    std::shared_ptr<estun::GraphicsPipeline> pipeline = render->CreatePipeline("shaders/main.vert.spv", "shaders/main.frag.spv", descriptor);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        context->StartDraw();

        render->StartDrawInCurrent();
        render->Bind(descriptor);
        render->Bind(pipeline);
        render->Bind(VB);
        render->Bind(IB);
        render->DrawIndexed(model.GetIndices().size(), 0, 0);
        render->RecordDrawInCurrent();

        ubo.view = camera.GetViewMatrix();
        ubo.projection = glm::perspective(glm::radians(camera.Zoom), (float)info.width_ / (float)info.height_, 0.1f, 10000.0f);
        UBs[context->GetImageIndex()].SetValue(ubo);

        context->SubmitDraw();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    render.reset();
    pipeline.reset();
    context->Clear();
    UBs.clear();
    VB.reset();
    IB.reset();
    descriptor.reset();
    context.reset();
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