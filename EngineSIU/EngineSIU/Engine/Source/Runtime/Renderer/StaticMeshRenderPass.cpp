#include "StaticMeshRenderPass.h"

#include "EngineLoop.h"
#include "World/World.h"

#include "RendererHelpers.h"
#include "Math/JungleMath.h"

#include "UObject/UObjectIterator.h"
#include "UObject/Casts.h"

#include "D3D11RHI/DXDBufferManager.h"
#include "D3D11RHI/GraphicDevice.h"
#include "D3D11RHI/DXDShaderManager.h"

#include "Components/StaticMeshComponent.h"

#include "BaseGizmos/GizmoBaseComponent.h"
#include "Engine/EditorEngine.h"
#include "LevelEditor/SLevelEditor.h"

#include "PropertyEditor/ShowFlags.h"

#include "UnrealEd/EditorViewportClient.h"


FStaticMeshRenderPass::FStaticMeshRenderPass() :
    Stride(0),
    BufferManager(nullptr),
    Graphics(nullptr),
    ShaderManager(nullptr),
    LightManager(nullptr)
{
}

FStaticMeshRenderPass::~FStaticMeshRenderPass()
{
    if (ShaderManager)
    {
        delete ShaderManager;
        ShaderManager = nullptr;
    }
}

void FStaticMeshRenderPass::CreateShader(EViewModeIndex viewMode)
{
    D3D11_INPUT_ELEMENT_DESC StaticMeshLayoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"MATERIAL_INDEX", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D11_INPUT_ELEMENT_DESC TextureLayoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    TArray<D3D_SHADER_MACRO> defines;
    // 조명 모델 이름 배열
    const char* lightModels[] = {"LIGHTING_MODEL_GOURAUD", "LIGHTING_MODEL_LAMBERT", "LIGHTING_MODEL_PHONG"};

    // 선택된 조명 모델만 1로 변경
    std::wstring vsKey;
    std::wstring psKey;
    
    if (viewMode >= Lit_Gouraud && viewMode <= Lit_Phong) {
        defines.Add({lightModels[viewMode], nullptr});
        // 동적 셰이더 키 생성
        vsKey = std::wstring(shortLightModels[viewMode]) + L"VS";
        psKey = std::wstring(shortLightModels[viewMode]) + L"PS";
    }

    if (viewMode >= Unlit)
    {
        vsKey = std::wstring(shortLightModels[Unlit]) + L"VS";
        psKey = std::wstring(shortLightModels[Unlit]) + L"PS";
    }
    
    Stride = sizeof(FStaticMeshVertex);
    ShaderManager->AddVertexShaderAndInputLayout(
        vsKey, L"Shaders/UberLit.hlsl", "Uber_VS", StaticMeshLayoutDesc, ARRAYSIZE(StaticMeshLayoutDesc), defines);

    ShaderManager->AddPixelShader(
        psKey, L"Shaders/UberLit.hlsl", "Uber_PS", defines);
    /*HRESULT hr = ShaderManager->AddVertexShaderAndInputLayout(L"StaticMeshVertexShader", L"Shaders/StaticMeshVertexShader.hlsl", "mainVS", StaticMeshLayoutDesc, ARRAYSIZE(StaticMeshLayoutDesc));

    hr = ShaderManager->AddPixelShader(L"StaticMeshPixelShader", L"Shaders/StaticMeshPixelShader.hlsl", "mainPS");

    VertexShader = ShaderManager->GetVertexShaderByKey(L"StaticMeshVertexShader");

    PixelShader = ShaderManager->GetPixelShaderByKey(L"StaticMeshPixelShader");

    InputLayout = ShaderManager->GetInputLayoutByKey(L"StaticMeshVertexShader");*/
}

void FStaticMeshRenderPass::ChangeViewMode(EViewModeIndex evi)
{
    switch (evi)
    {
    case EViewModeIndex::Lit_Gouraud:
    case EViewModeIndex::Lit_Lambert:
    case EViewModeIndex::Lit_Phong:
        UpdateLitUnlitConstant(1);
        break;
    case EViewModeIndex::Wireframe:
    case EViewModeIndex::Unlit:
        UpdateLitUnlitConstant(0);
        break;
    }
}


void FStaticMeshRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    BufferManager = InBufferManager;
    Graphics = InGraphics;
    ShaderManager = InShaderManager;
}

void FStaticMeshRenderPass::PrepareRender()
{
    for (const auto iter : TObjectRange<UStaticMeshComponent>())
    {
        if (!Cast<UGizmoBaseComponent>(iter) && iter->GetWorld() == GEngine->ActiveWorld)
        {
            StaticMeshObjs.Add(iter);
        }
    }
}

void FStaticMeshRenderPass::ChangeShader(EViewModeIndex viewMode)
{
    std::wstring vsKey;
    std::wstring psKey;
    if (viewMode >= Lit_Gouraud && viewMode <= Unlit) {
        int index = viewMode - Lit_Gouraud; // 인덱스 계산 (열거형 값에 따라 조정 필요)
        // 동적 셰이더 키 생성
        vsKey = std::wstring(shortLightModels[index]) + L"VS";
        psKey = std::wstring(shortLightModels[index]) + L"PS";
	}
	else 
	{
		vsKey = L"UnlitVS";
		psKey = L"UnlitPS";
	}

    if (ShaderManager->GetVertexShaderByKey(vsKey) == nullptr || ShaderManager->GetPixelShaderByKey(psKey) == nullptr)
        CreateShader(viewMode);

    Graphics->DeviceContext->VSSetShader(ShaderManager->GetVertexShaderByKey(vsKey), nullptr, 0);
    Graphics->DeviceContext->PSSetShader(ShaderManager->GetPixelShaderByKey(psKey), nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(ShaderManager->GetInputLayoutByKey(vsKey));
}

void FStaticMeshRenderPass::PrepareRenderState(EViewModeIndex viewMode)
{
    ChangeShader(viewMode);
    
    // 상수 버퍼 바인딩 예시
    /*
    ID3D11Buffer* PerObjectBuffer = BufferManager->GetConstantBuffer(TEXT("FPerObjectConstantBuffer"));
    ID3D11Buffer* CameraConstantBuffer = BufferManager->GetConstantBuffer(TEXT("FCameraConstantBuffer"));
    ID3D11Buffer* LightConstantBuffer = BufferManager->GetConstantBuffer(TEXT("FLightBuffer"));
    Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &PerObjectBuffer);
    Graphics->DeviceContext->VSSetConstantBuffers(1, 1, &CameraConstantBuffer);
    Graphics->DeviceContext->VSSetConstantBuffers(2, 1, &LightConstantBuffer);
    Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &PerObjectBuffer);

    TArray<FString> PSBufferKeys = {
                                  TEXT("FCameraConstantBuffer"),
                                  TEXT("FLightBuffer"),
                                  TEXT("FMaterialConstants"),
                                  TEXT("FLitUnlitConstants"),
                                  TEXT("FSubMeshConstants"),
                                  TEXT("FTextureConstants")
    };*/
    TArray<FString> VSBufferKeys = {
        TEXT("FLightBuffer"),
    };
    BufferManager->BindConstantBuffers(VSBufferKeys, 0, EShaderStage::Vertex);
    TArray<FString> PSBufferKeys = {
                              TEXT("FLightBuffer"),
                              TEXT("FLitUnlitConstants"),
                              TEXT("FSubMeshConstants"),
                              TEXT("FTextureConstants")
    };
    BufferManager->BindConstantBuffers(PSBufferKeys, 0, EShaderStage::Pixel);
}

void FStaticMeshRenderPass::UpdatePerObjectConstant(const FMatrix& Model, const FMatrix& View, const FMatrix& Projection, const FVector4& UUIDColor, bool Selected) const
{
    FMatrix NormalMatrix = RendererHelpers::CalculateNormalMatrix(Model);
    //FPerObjectConstantBuffer Data(Model, NormalMatrix, UUIDColor, Selected);
    FMatrix MInverseTranspose = RendererHelpers::CalculateNormalMatrix(Model); // [변경됨]
    FPerObjectConstantBuffer Data(Model, View, Projection, MInverseTranspose);
    BufferManager->UpdateConstantBuffer(TEXT("FPerObjectConstantBuffer"), Data);
   
}

void FStaticMeshRenderPass::UpdateLitUnlitConstant(int isLit) const
{
    FLitUnlitConstants Data;
    Data.isLit = isLit;
    BufferManager->UpdateConstantBuffer(TEXT("FLitUnlitConstants"), Data);
}


void FStaticMeshRenderPass::RenderPrimitive(OBJ::FStaticMeshRenderData* RenderData, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int SelectedSubMeshIndex) const
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


void FStaticMeshRenderPass::RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
    Graphics->DeviceContext->Draw(numVertices, 0);
}

void FStaticMeshRenderPass::RenderPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void FStaticMeshRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    if (!(Viewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Primitives))) return;

    PrepareRenderState(Viewport->ViewMode);

    for (UStaticMeshComponent* Comp : StaticMeshObjs) {
        if (!Comp || !Comp->GetStaticMesh()) continue;
        
        FMatrix Model = Comp->GetWorldMatrix();

        FVector4 UUIDColor = Comp->EncodeUUID() / 255.0f;

        UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
        bool Selected = (Engine && Engine->GetSelectedActor() == Comp->GetOwner());

        UpdatePerObjectConstant(Model, Viewport->GetViewMatrix(), Viewport->GetProjectionMatrix(), UUIDColor, Selected);
        //FCameraConstantBuffer CameraData(Viewport->GetViewMatrix(), Viewport->GetProjectionMatrix(), Viewport->ViewTransformPerspective.GetLocation(), 0);
        FCameraConstantBuffer CameraData(Viewport->ViewTransformPerspective.GetLocation(), 0);
        BufferManager->UpdateConstantBuffer(TEXT("FCameraConstantBuffer"), CameraData);

        OBJ::FStaticMeshRenderData* RenderData = Comp->GetStaticMesh()->GetRenderData();

        if (RenderData == nullptr) continue;

        RenderPrimitive(RenderData, Comp->GetStaticMesh()->GetMaterials(), Comp->GetOverrideMaterials(), Comp->GetselectedSubMeshIndex());

        if (Viewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_AABB))
        {
            FEngineLoop::PrimitiveDrawBatch.AddAABBToBatch(Comp->GetBoundingBox(), Comp->GetWorldLocation(), Model);
        }
    }
}

void FStaticMeshRenderPass::ClearRenderArr()
{
    StaticMeshObjs.Empty();
}

