#include "LightRenderPass.h"

#include "Components/Light/AmbientLightComponent.h"
#include "Components/Light/LightComponentBase.h"
#include "Components/Light/PointLightComponent.h"
#include "Components/Light/SpotLightComponent.h"

#include "UObject/UObjectIterator.h"
#include "Engine/Engine.h"
#include "UObject/Casts.h"

FLightRenderPass::FLightRenderPass()
    : Graphics(nullptr), BufferManager(nullptr), ShaderManager(nullptr)
{}

void FLightRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    Graphics = InGraphics;
    BufferManager = InBufferManager;
    ShaderManager = InShaderManager;

    
}

void FLightRenderPass::PrepareRender()
{
    for (ULightComponentBase* LightIter : TObjectRange<ULightComponentBase>())
    {
        if (LightIter->GetWorld() == GEngine->ActiveWorld)
        {
            // 각 LightComponent 별로 Array 추가.
            if (Cast<UPointLightComponent>(LightIter))
            {
                // PointLightArray.Add(LightIter);
            }
            else if (Cast<USpotLightComponent>(LightIter))
            {
                // SpotLightArray.Add(LightIter);
            }
            else if (Cast<UAmbientLightComponent>(LightIter))
            {
                // AmbientLightArray.Add(LightIter);
            }
        }
    }
}

void FLightRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}

void FLightRenderPass::ClearRenderArr()
{
}
