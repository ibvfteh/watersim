#include "renderer/render.h"
#include "renderer/context.h"

estun::Render::Render(bool toDefault)
    : toDefault_(toDefault)
{
    Create();
}

estun::Render::~Render()
{
    pipelines_.clear();
    Destroy();
}

void estun::Render::Create()
{
    auto msaa = ContextLocator::GetContext()->GetMsaaSamples();
    bool hasMsaa = msaa != VK_SAMPLE_COUNT_1_BIT;
    if (hasMsaa)
    {
        ES_CORE_INFO("Render MSAA is on");
    }
    else
    {
        ES_CORE_INFO("Render MSAA is off");
    }
    auto size = static_cast<uint32_t>(ContextLocator::GetSwapChain()->GetImages().size());
    commandBuffers_.reset(new CommandBuffers(CommandPoolLocator::GetGraphicsPool(), size));
    ES_CORE_INFO("Command buffers done");

    colorResources_.reset(new ColorResources(ContextLocator::GetSwapChain()->GetExtent(), msaa));
    ES_CORE_INFO("Color resources done");

    depthResources_.reset(new DepthResources(ContextLocator::GetSwapChain()->GetExtent(), msaa));
    ES_CORE_INFO("Depth resources done");

    if (hasMsaa)
    {
        colorResolveResources_.reset(new ColorResources(ContextLocator::GetSwapChain()->GetExtent(), msaa));
        ES_CORE_INFO("Color resolve resources done");
    }

    renderPass_.reset(new RenderPass(hasMsaa));
    ES_CORE_INFO("Render pass done");

    for (int i = 0; i < size; i++)
    {
        std::vector<ImageView *> attachments;
        if (toDefault_)
        {
            attachments.push_back(ContextLocator::GetSwapChain()->GetImageViews()[i].get());
        }
        else
        {
            attachments.push_back(colorResources_->GetImageView().get());
        }
        attachments.push_back(depthResources_->GetImageView().get());

        if (hasMsaa)
        {
            attachments.push_back(colorResolveResources_->GetImageView().get());
        }

        framebuffers_.push_back(Framebuffer(attachments, renderPass_));
    }
    ES_CORE_INFO("Framebuffers done");
}

void estun::Render::Destroy()
{
    framebuffers_.clear();
    renderPass_.reset();
    colorResolveResources_.reset();
    depthResources_.reset();
    colorResources_.reset();
    commandBuffers_.reset();
}

void estun::Render::Recreate()
{
    Destroy();
    Create();
    for (auto & pipeline : pipelines_)
    {
        pipeline->Recreate(renderPass_);
    }
}

std::shared_ptr<estun::GraphicsPipeline> estun::Render::CreatePipeline(
        const std::string vertexShaderName,
        const std::string fragmentShaderName,
        const std::shared_ptr<Descriptor> descriptor)
{
    std::shared_ptr<GraphicsPipeline> pipeline = std::make_shared<GraphicsPipeline>(vertexShaderName, fragmentShaderName, renderPass_, descriptor,  ContextLocator::GetContext()->GetMsaaSamples(), false);
    pipelines_.push_back(pipeline);
    return pipeline;
}

void estun::Render::StartDrawInCurrent()
{
    commandBuffers_->Begin(ContextLocator::GetImageIndex());
    renderPass_->Begin(framebuffers_[ContextLocator::GetImageIndex()], GetCurrCommandBuffer());
}

void estun::Render::RecordDrawInCurrent()
{
    renderPass_->End(GetCurrCommandBuffer());
    commandBuffers_->End(ContextLocator::GetImageIndex());
}

void estun::Render::Bind(std::shared_ptr<Descriptor> descriptor)
{
    descriptor->Bind(GetCurrCommandBuffer());
}

void estun::Render::Bind(std::shared_ptr<GraphicsPipeline> pipeline)
{
    pipeline->Bind(GetCurrCommandBuffer());
}

void estun::Render::Bind(std::shared_ptr<VertexBuffer> vertexBuffer)
{
    vertexBuffer->Bind(GetCurrCommandBuffer());
}

void estun::Render::Bind(std::shared_ptr<IndexBuffer> indexBuffer)
{
    indexBuffer->Bind(GetCurrCommandBuffer());
}

VkCommandBuffer &estun::Render::GetCurrCommandBuffer()
{
    return (*commandBuffers_)[ContextLocator::GetImageIndex()];
}

void estun::Render::DrawIndexed(uint32_t indexesSize, uint32_t indexOffset, uint32_t vertexOffset)
{
    vkCmdDrawIndexed(GetCurrCommandBuffer(), indexesSize, 1, indexOffset, vertexOffset, 0);
}
