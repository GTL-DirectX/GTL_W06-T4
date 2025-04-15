#pragma once
#include "IRenderPass.h"
#include "EngineBaseTypes.h"

#include "Define.h"

class FLightManager;
class FDXDShaderManager;
class UWorld;
class UMaterial;
class FEditorViewportClient;
class UStaticMeshComponent;
struct FStaticMaterial;

class FStaticMeshRenderPass : public IRenderPass
{
public:
    FStaticMeshRenderPass();
    
    ~FStaticMeshRenderPass();
    
    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    
    virtual void PrepareRender() override;

    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    virtual void ClearRenderArr() override;

    void SetLightManager(FLightManager* InLightManager) { LightManager = InLightManager; };

    void PrepareRenderState(EViewModeIndex viewMode);
    
    void UpdatePerObjectConstant(const FMatrix& Model, const FMatrix& View, const FMatrix& Projection, const FVector4& UUIDColor, bool Selected) const;
  
    void UpdateLitUnlitConstant(int isLit) const;

    void RenderPrimitive(OBJ::FStaticMeshRenderData* RenderData, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int SelectedSubMeshIndex) const;
    
    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices) const;

    void RenderPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices) const;

    // Shader 관련 함수 (생성/해제 등)
    void CreateShader(EViewModeIndex viewMode);
    void ChangeShader(EViewModeIndex evi);

    void ChangeViewMode(EViewModeIndex evi);
private:
    TArray<UStaticMeshComponent*> StaticMeshObjs;
    
    uint32 Stride;

    FDXDBufferManager* BufferManager;
    
    FGraphicsDevice* Graphics;
    
    FDXDShaderManager* ShaderManager;

    FLightManager* LightManager;
    // 셰이더 키에 사용할 짧은 모델 이름 배열
    const wchar_t* shortLightModels[4] = {L"Gouraud", L"Lambert", L"Phong", L"Unlit"};
};
