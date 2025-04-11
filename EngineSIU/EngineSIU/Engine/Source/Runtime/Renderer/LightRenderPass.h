#pragma once

#include "IRenderPass.h"

class FLightRenderPass : public IRenderPass
{
    
public:
    FLightRenderPass();
    virtual ~FLightRenderPass() override = default;
    
    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    virtual void PrepareRender() override;
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void ClearRenderArr() override;

private:
    // DirectX 접근 기본 클래스.
    FGraphicsDevice* Graphics;
    FDXDBufferManager* BufferManager;
    FDXDShaderManager* ShaderManager;
    
};
