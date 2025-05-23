
#include "Renderer.h"
#include "World/World.h"
#include "UnrealEd/EditorViewportClient.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "StaticMeshRenderPass.h"
#include "WorldNormalRenderPass.h"
#include "BillboardRenderPass.h"
#include "GizmoRenderPass.h"
//#include "UpdateLightBufferPass.h"
#include "LineRenderPass.h"
#include "DepthBufferDebugPass.h"
#include "FogRenderPass.h"
#include <UObject/UObjectIterator.h>

#include "LightType.h"
#include "GameFrameWork/Actor.h"

//------------------------------------------------------------------------------
// 초기화 및 해제 관련 함수
//------------------------------------------------------------------------------
void FRenderer::Initialize(FGraphicsDevice* InGraphics, FDXDBufferManager* InBufferManager)
{
    Graphics = InGraphics;
    BufferManager = InBufferManager;

    ShaderManager = new FDXDShaderManager(Graphics->Device);
    StaticMeshRenderPass = new FStaticMeshRenderPass();
    WorldNormalRenderPass = new FWorldNormalRenderPass();
    BillboardRenderPass = new FBillboardRenderPass();
    GizmoRenderPass = new FGizmoRenderPass();
    //UpdateLightBufferPass = new FUpdateLightBufferPass();
    LineRenderPass = new FLineRenderPass();
    DepthBufferDebugPass = new FDepthBufferDebugPass();
    FogRenderPass = new FFogRenderPass();
    LightManager = new FLightManager();

    StaticMeshRenderPass->Initialize(BufferManager, Graphics, ShaderManager);
    StaticMeshRenderPass->SetLightManager(LightManager);
    BillboardRenderPass->Initialize(BufferManager, Graphics, ShaderManager);
    GizmoRenderPass->Initialize(BufferManager, Graphics, ShaderManager);
    //UpdateLightBufferPass->Initialize(BufferManager, Graphics, ShaderManager);
    LineRenderPass->Initialize(BufferManager, Graphics, ShaderManager);
    DepthBufferDebugPass->Initialize(BufferManager, Graphics, ShaderManager);
    FogRenderPass->Initialize(BufferManager, Graphics, ShaderManager);
    WorldNormalRenderPass->Initialize(BufferManager, Graphics, ShaderManager);
    LightManager->Initialize(BufferManager);
    CreateConstantBuffers();
}

void FRenderer::Release()
{
}


void FRenderer::ChangeViewMode(EViewModeIndex evi)
{
    StaticMeshRenderPass->ChangeViewMode(evi);
    if (evi == EViewModeIndex::SceneDepth)
        IsSceneDepth = true;
    else
        IsSceneDepth = false;
}

//------------------------------------------------------------------------------
// 사용하는 모든 상수 버퍼 생성
//------------------------------------------------------------------------------
void FRenderer::CreateConstantBuffers()
{
    UINT perObjectBufferSize = sizeof(FPerObjectConstantBuffer);
    BufferManager->CreateBufferGeneric<FPerObjectConstantBuffer>("FPerObjectConstantBuffer", nullptr, perObjectBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    UINT cameraConstantBufferSize = sizeof(FCameraConstantBuffer);
    BufferManager->CreateBufferGeneric<FCameraConstantBuffer>("FCameraConstantBuffer", nullptr, cameraConstantBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    UINT subUVBufferSize = sizeof(FSubUVConstant);
    BufferManager->CreateBufferGeneric<FSubUVConstant>("FSubUVConstant", nullptr, subUVBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    UINT materialBufferSize = sizeof(FMaterialConstants);
    BufferManager->CreateBufferGeneric<FMaterialConstants>("FMaterialConstants", nullptr, materialBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    UINT subMeshBufferSize = sizeof(FSubMeshConstants);
    BufferManager->CreateBufferGeneric<FSubMeshConstants>("FSubMeshConstants", nullptr, subMeshBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    UINT textureBufferSize = sizeof(FTextureConstants);
    BufferManager->CreateBufferGeneric<FTextureConstants>("FTextureConstants", nullptr, textureBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    UINT lightingBufferSize = sizeof(FLightBuffer);
    BufferManager->CreateBufferGeneric<FLightBuffer>("FLightBuffer", nullptr, lightingBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    
    UINT litUnlitBufferSize = sizeof(FLitUnlitConstants);
    BufferManager->CreateBufferGeneric<FLitUnlitConstants>("FLitUnlitConstants", nullptr, litUnlitBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    UINT ScreenConstantsBufferSize = sizeof(FScreenConstants);
    BufferManager->CreateBufferGeneric<FScreenConstants>("FScreenConstants", nullptr, ScreenConstantsBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    UINT FogConstantBufferSize = sizeof(FFogConstants);
    BufferManager->CreateBufferGeneric<FFogConstants>("FFogConstants", nullptr, FogConstantBufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void FRenderer::ReleaseConstantBuffer()
{
    BufferManager->ReleaseConstantBuffer();
}
void FRenderer::BindGlobalConstantBuffers()
{
    TArray<FString> Keys = {
        TEXT("FPerObjectConstantBuffer"),
        TEXT("FCameraConstantBuffer"),
        TEXT("FMaterialConstants")
    };
    BufferManager->BindConstantBuffers(Keys, /*StartSlot=*/10, EShaderStage::Vertex);
    BufferManager->BindConstantBuffers(Keys, /*StartSlot=*/10, EShaderStage::Pixel);
}

void FRenderer::PrepareRender()
{
    StaticMeshRenderPass->PrepareRender();
    WorldNormalRenderPass->PrepareRender();
    GizmoRenderPass->PrepareRender();
    BillboardRenderPass->PrepareRender();
    //UpdateLightBufferPass->PrepareRender();
    FogRenderPass->PrepareRender();
}

void FRenderer::ClearRenderArr()
{
    StaticMeshRenderPass->ClearRenderArr();
    WorldNormalRenderPass->ClearRenderArr();
    BillboardRenderPass->ClearRenderArr();
    GizmoRenderPass->ClearRenderArr();
    //UpdateLightBufferPass->ClearRenderArr();
    FogRenderPass->ClearRenderArr();
}

void FRenderer::Render(const std::shared_ptr<FEditorViewportClient>& ActiveViewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &ActiveViewport->GetD3DViewport());

    Graphics->ChangeRasterizer(ActiveViewport->GetViewMode());

    ChangeViewMode(ActiveViewport->GetViewMode());
    BindGlobalConstantBuffers();
    LightManager->UpdateLightBuffer(ActiveViewport);

    if (ActiveViewport->GetViewMode() == EViewModeIndex::WorldNormal) {
        WorldNormalRenderPass->Render(ActiveViewport);
    }
    else 
    {
        StaticMeshRenderPass->Render(ActiveViewport);
    }
    LightManager->VisualizeLights(&GEngineLoop.PrimitiveDrawBatch);

    //UpdateLightBufferPass->Render(ActiveViewport);
    BillboardRenderPass->Render(ActiveViewport);
    

    if (IsSceneDepth)
    {
        DepthBufferDebugPass->RenderDepthBuffer(ActiveViewport);
    }

    if (!IsSceneDepth)
    {
        DepthBufferDebugPass->UpdateDepthBufferSRV();
        
        FogRenderPass->RenderFog(ActiveViewport, DepthBufferDebugPass->GetDepthSRV());
    }
    LineRenderPass->Render(ActiveViewport);
    GizmoRenderPass->Render(ActiveViewport);

    ClearRenderArr();
}

void FRenderer::HotReload()
{
    ShaderManager->HotReloadShader();
}
