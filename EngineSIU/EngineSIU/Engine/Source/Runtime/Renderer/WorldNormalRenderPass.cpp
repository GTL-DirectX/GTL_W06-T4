// WorldNormalRenderPass.cpp
#include "WorldNormalRenderPass.h"
#include "EngineLoop.h"
#include "World/World.h"
#include "RendererHelpers.h"
#include "Math/JungleMath.h"
#include "UObject/UObjectIterator.h"
#include "UObject/Casts.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Engine/EditorEngine.h"
#include "PropertyEditor/ShowFlags.h"

FWorldNormalRenderPass::FWorldNormalRenderPass()
    : Stride(0)
    , BufferManager(nullptr)
    , Graphics(nullptr)
    , ShaderManager(nullptr)
    , VertexShader(nullptr)
    , PixelShader(nullptr)
    , InputLayout(nullptr)
{
}

FWorldNormalRenderPass::~FWorldNormalRenderPass()
{
    // 리소스 해제 코드
}

void FWorldNormalRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    Graphics = InGraphics;
    BufferManager = InBufferManager;
    ShaderManager = InShaderManager;
    CreateShader();
}

void FWorldNormalRenderPass::CreateShader()
{
    D3D11_INPUT_ELEMENT_DESC StaticMeshLayoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"MATERIAL_INDEX", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    Stride = sizeof(FStaticMeshVertex);

    // 셰이더 컴파일
    ShaderManager->AddVertexShaderAndInputLayout(
        L"WorldNormalVS", L"Shaders/WorldNormalVertexShader.hlsl", "mainVS", StaticMeshLayoutDesc, ARRAYSIZE(StaticMeshLayoutDesc));

    ShaderManager->AddPixelShader(
        L"WorldNormalPS", L"Shaders/WorldNormalPixelShader.hlsl", "mainPS");

    VertexShader = ShaderManager->GetVertexShaderByKey(L"WorldNormalVS");
    PixelShader = ShaderManager->GetPixelShaderByKey(L"WorldNormalPS");
    InputLayout = ShaderManager->GetInputLayoutByKey(L"WorldNormalVS");
}

void FWorldNormalRenderPass::PrepareRender()
{
    for (const auto iter : TObjectRange<UStaticMeshComponent>())
    {
        if (!Cast<UGizmoBaseComponent>(iter) && iter->GetWorld() == GEngine->ActiveWorld)
        {
            StaticMeshObjs.Add(iter);
        }
    }
}

void FWorldNormalRenderPass::PrepareRenderState()
{
    Graphics->DeviceContext->VSSetShader(ShaderManager->GetVertexShaderByKey(L"WorldNormalVS"), nullptr, 0);
    Graphics->DeviceContext->PSSetShader(ShaderManager->GetPixelShaderByKey(L"WorldNormalPS"), nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(ShaderManager->GetInputLayoutByKey(L"WorldNormalVS"));

    TArray<FString> VSBufferKeys = {
        TEXT("FLightBuffer"),
    };
    BufferManager->BindConstantBuffers(VSBufferKeys, 0, EShaderStage::Vertex);

    TArray<FString> PSBufferKeys = {
        TEXT("FLightBuffer"),
        TEXT("FSubMeshConstants"),
        TEXT("FTextureConstants")
    };
    BufferManager->BindConstantBuffers(PSBufferKeys, 0, EShaderStage::Pixel);
}

void FWorldNormalRenderPass::UpdatePerObjectConstant(const FMatrix& Model, const FMatrix& View, const FMatrix& Projection, const FVector4& UUIDColor, bool Selected) const
{
    FMatrix MInverseTranspose = RendererHelpers::CalculateNormalMatrix(Model);
    FPerObjectConstantBuffer Data(Model, View, Projection, MInverseTranspose);
    BufferManager->UpdateConstantBuffer(TEXT("FPerObjectConstantBuffer"), Data);
}

void FWorldNormalRenderPass::RenderPrimitive(OBJ::FStaticMeshRenderData* RenderData, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int SelectedSubMeshIndex) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &RenderData->VertexBuffer, &Stride, &offset);
    if (RenderData->IndexBuffer)
        Graphics->DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    if (RenderData->MaterialSubsets.Num() == 0) {
        Graphics->DeviceContext->DrawIndexed(RenderData->Indices.Num(), 0, 0);
        return;
    }

    for (int subMeshIndex = 0; subMeshIndex < RenderData->MaterialSubsets.Num(); subMeshIndex++) {
        int materialIndex = RenderData->MaterialSubsets[subMeshIndex].MaterialIndex;

        FSubMeshConstants SubMeshData = (subMeshIndex == SelectedSubMeshIndex) ? FSubMeshConstants(true) : FSubMeshConstants(false);
        BufferManager->UpdateConstantBuffer(TEXT("FSubMeshConstants"), SubMeshData);

        if (OverrideMaterials[materialIndex] != nullptr)
            MaterialUtils::UpdateMaterial(BufferManager, Graphics, OverrideMaterials[materialIndex]->GetMaterialInfo());
        else
            MaterialUtils::UpdateMaterial(BufferManager, Graphics, Materials[materialIndex]->Material->GetMaterialInfo());

        uint64 startIndex = RenderData->MaterialSubsets[subMeshIndex].IndexStart;
        uint64 indexCount = RenderData->MaterialSubsets[subMeshIndex].IndexCount;
        Graphics->DeviceContext->DrawIndexed(indexCount, startIndex, 0);
    }
}

void FWorldNormalRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    if (!(Viewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Primitives))) return;

    PrepareRenderState();

    for (UStaticMeshComponent* Comp : StaticMeshObjs) {
        if (!Comp || !Comp->GetStaticMesh()) continue;

        FMatrix Model = Comp->GetWorldMatrix();
        FVector4 UUIDColor = Comp->EncodeUUID() / 255.0f;

        UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
        bool Selected = (Engine && Engine->GetSelectedActor() == Comp->GetOwner());

        UpdatePerObjectConstant(Model, Viewport->GetViewMatrix(), Viewport->GetProjectionMatrix(), UUIDColor, Selected);

        FCameraConstantBuffer CameraData(Viewport->ViewTransformPerspective.GetLocation(), 0);
        BufferManager->UpdateConstantBuffer(TEXT("FCameraConstantBuffer"), CameraData);

        // 상수 버퍼 바인딩
        TArray<FString> VSBufferKeys = {
            TEXT("FPerObjectConstantBuffer"),
            TEXT("FCameraConstantBuffer")
        };
        BufferManager->BindConstantBuffers(VSBufferKeys, 0, EShaderStage::Vertex);

        TArray<FString> PSBufferKeys = {
            TEXT("FPerObjectConstantBuffer"),
            TEXT("FCameraConstantBuffer"),
            TEXT("FMaterialConstants")
        };
        BufferManager->BindConstantBuffers(PSBufferKeys, 0, EShaderStage::Pixel);

        OBJ::FStaticMeshRenderData* RenderData = Comp->GetStaticMesh()->GetRenderData();
        if (RenderData == nullptr) continue;

        RenderPrimitive(RenderData, Comp->GetStaticMesh()->GetMaterials(), Comp->GetOverrideMaterials(), Comp->GetselectedSubMeshIndex());
    }
}

void FWorldNormalRenderPass::ClearRenderArr()
{
    StaticMeshObjs.Empty();
}
