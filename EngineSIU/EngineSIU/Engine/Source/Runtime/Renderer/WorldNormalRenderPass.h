#pragma once

#include "D3D11RHI/GraphicDevice.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "D3D11RHI/DXDBufferManager.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Components/StaticMeshComponent.h"

class FWorldNormalRenderPass
{
public:
    FWorldNormalRenderPass();
    ~FWorldNormalRenderPass();

    void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager);
    void PrepareRender();
    void Render(const std::shared_ptr<FEditorViewportClient>& Viewport);
    void ClearRenderArr();

private:
    void CreateShader();
    void PrepareRenderState();
    void UpdatePerObjectConstant(const FMatrix& Model, const FMatrix& View, const FMatrix& Projection, const FVector4& UUIDColor, bool Selected) const;
    void RenderPrimitive(OBJ::FStaticMeshRenderData* RenderData, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int SelectedSubMeshIndex) const;

    FGraphicsDevice* Graphics;
    FDXDBufferManager* BufferManager;
    FDXDShaderManager* ShaderManager;

    UINT Stride;
    TArray<UStaticMeshComponent*> StaticMeshObjs;
};
