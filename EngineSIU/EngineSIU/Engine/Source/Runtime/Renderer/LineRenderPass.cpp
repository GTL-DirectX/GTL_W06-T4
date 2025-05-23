#include "LineRenderPass.h"

#include "D3D11RHI/DXDBufferManager.h"

#include "D3D11RHI/GraphicDevice.h"

#include "D3D11RHI/DXDShaderManager.h"

#include "RendererHelpers.h"

#include "Math/JungleMath.h"

#include "EngineLoop.h"

#include "UObject/UObjectIterator.h"

#include "UnrealEd/EditorViewportClient.h"

// 생성자/소멸자
FLineRenderPass::FLineRenderPass()
    : BufferManager(nullptr)
    , Graphics(nullptr)
    , ShaderManager(nullptr)
    , VertexLineShader(nullptr)
    , PixelLineShader(nullptr)
{
}

FLineRenderPass::~FLineRenderPass()
{
    // 셰이더 자원 해제
    FDXDBufferManager::SafeRelease(VertexLineShader);
    FDXDBufferManager::SafeRelease(PixelLineShader);
}

void FLineRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    BufferManager = InBufferManager;
    Graphics = InGraphics;
    ShaderManager = InShaderManager;
    CreateShader();
}

void FLineRenderPass::PrepareRender()
{
}

void FLineRenderPass::ClearRenderArr()
{
    // 필요에 따라 내부 배열을 초기화
}

void FLineRenderPass::CreateShader()
{
    HRESULT hr = ShaderManager->AddVertexShader(L"VertexLineShader", L"Shaders/ShaderLine.hlsl", "mainVS");
    hr = ShaderManager->AddPixelShader(L"PixelLineShader", L"Shaders/ShaderLine.hlsl", "mainPS");

    VertexLineShader = ShaderManager->GetVertexShaderByKey(L"VertexLineShader");
    PixelLineShader = ShaderManager->GetPixelShaderByKey(L"PixelLineShader");
}

void FLineRenderPass::PrepareLineShader() const
{
    Graphics->DeviceContext->VSSetShader(ShaderManager->GetVertexShaderByKey(L"VertexLineShader"), nullptr, 0);
    Graphics->DeviceContext->PSSetShader(ShaderManager->GetPixelShaderByKey(L"PixelLineShader"), nullptr, 0);

    /*
    BufferManager->BindConstantBuffer(TEXT("FPerObjectConstantBuffer"), 0, EShaderStage::Vertex);
    BufferManager->BindConstantBuffer(TEXT("FPerObjectConstantBuffer"), 0, EShaderStage::Pixel);
    BufferManager->BindConstantBuffer(TEXT("FCameraConstantBuffer"), 2, EShaderStage::Pixel);
    */

    FEngineLoop::PrimitiveDrawBatch.PrepareLineResources();
}

void FLineRenderPass::DrawLineBatch(const FLinePrimitiveBatchArgs& BatchArgs) const
{
    UINT stride = sizeof(FSimpleVertex);
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &BatchArgs.VertexBuffer, &stride, &offset);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    const UINT vertexCountPerInstance = 2;
    UINT instanceCount = BatchArgs.GridParam.NumGridLines + 3 +
        (BatchArgs.BoundingBoxCount * 12) +
        (BatchArgs.ConeCount * (2 * BatchArgs.ConeSegmentCount)) +
        (12 * BatchArgs.OBBCount) + (BatchArgs.SphereCount * 3 * BatchArgs.SphereSegmentCount);

    Graphics->DeviceContext->DrawInstanced(vertexCountPerInstance, instanceCount, 0, 0);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FLineRenderPass::ProcessLineRendering(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareLineShader();
    FMatrix Model = FMatrix::Identity;
    FMatrix View = Viewport->GetViewMatrix();
    FMatrix Projection = Viewport->GetProjectionMatrix();
    // 상수 버퍼 업데이트: Identity 모델, 기본 색상 등
    /*FMatrix MVP = RendererHelpers::CalculateMVP(FMatrix::Identity, Viewport->GetViewMatrix(), Viewport->GetProjectionMatrix());
    FMatrix NormalMatrix = RendererHelpers::CalculateNormalMatrix(FMatrix::Identity);*/
    FMatrix MInverseTranspose = RendererHelpers::CalculateNormalMatrix(Model); // [변경됨]
    FPerObjectConstantBuffer Data(Model, View, Projection, MInverseTranspose);
    BufferManager->UpdateConstantBuffer(TEXT("FPerObjectConstantBuffer"), Data);
    FCameraConstantBuffer CameraData(Viewport->ViewTransformPerspective.GetLocation());

    BufferManager->UpdateConstantBuffer(TEXT("FCameraConstantBuffer"), CameraData);
    FLinePrimitiveBatchArgs BatchArgs;
    FEngineLoop::PrimitiveDrawBatch.PrepareBatch(BatchArgs);
    DrawLineBatch(BatchArgs);
    FEngineLoop::PrimitiveDrawBatch.RemoveArr();
}

void FLineRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->OMSetRenderTargets(1, &Graphics->FrameBufferRTV, Graphics->DepthStencilView);

    ProcessLineRendering(Viewport);


  
}
