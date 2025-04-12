#pragma once
#include "Launch/EngineLoop.h"
#include "Components/Material/MaterialTypes.h"

namespace RendererHelpers {
    
    inline FMatrix CalculateMVP(const FMatrix& Model, const FMatrix& View, const FMatrix& Projection) {
        return Model * View * Projection;
    }

    inline FMatrix CalculateNormalMatrix(const FMatrix& Model) {
        return FMatrix::Transpose(FMatrix::Inverse(Model));
    }
}

namespace MaterialUtils {
    inline void UpdateMaterial(FDXDBufferManager* BufferManager, FGraphicsDevice* Graphics, const FObjMaterialInfo& MaterialInfo) {
        FMaterialConstants data;
        data.DiffuseColor = MaterialInfo.Diffuse;
        data.TransparencyScalar = MaterialInfo.TransparencyScalar;
        data.AmbientColor = MaterialInfo.Ambient;
        data.DensityScalar = MaterialInfo.DensityScalar;
        data.SpecularColor = MaterialInfo.Specular;
        data.SpecularScalar = MaterialInfo.SpecularScalar;
        data.EmmisiveColor = MaterialInfo.Emissive;
        data.TextureSlotMask = MaterialInfo.TextureSlotMask;

        BufferManager->UpdateConstantBuffer(TEXT("FMaterialConstants"), data);

        if (MaterialInfo.TextureSlotMask & MaterialTextureFlags::Diffuse) {
            std::shared_ptr<FTexture> texture = FEngineLoop::ResourceManager.GetTexture(MaterialInfo.DiffuseTexturePath);
            Graphics->DeviceContext->PSSetShaderResources(ETextureSlot::Diffuse, 1, &texture->TextureSRV);
            // TODO : Sampler state 분리
            Graphics->DeviceContext->PSSetSamplers(0, 1, &texture->SamplerState);
        }

        if (MaterialInfo.TextureSlotMask & MaterialTextureFlags::Ambient) {
            std::shared_ptr<FTexture> texture = FEngineLoop::ResourceManager.GetTexture(MaterialInfo.AmbientTexturePath);
            Graphics->DeviceContext->PSSetShaderResources(ETextureSlot::Ambient, 1, &texture->TextureSRV);
            Graphics->DeviceContext->PSSetSamplers(0, 1, &texture->SamplerState);
        }
        
        if (MaterialInfo.TextureSlotMask & MaterialTextureFlags::Specular) {
            std::shared_ptr<FTexture> texture = FEngineLoop::ResourceManager.GetTexture(MaterialInfo.SpecularTexturePath);
            Graphics->DeviceContext->PSSetShaderResources(ETextureSlot::Specular, 1, &texture->TextureSRV);
            Graphics->DeviceContext->PSSetSamplers(0, 1, &texture->SamplerState);
        }

        if (MaterialInfo.TextureSlotMask & MaterialTextureFlags::Bump) {
            std::shared_ptr<FTexture> texture = FEngineLoop::ResourceManager.GetTexture(MaterialInfo.BumpTexturePath);
            Graphics->DeviceContext->PSSetShaderResources(ETextureSlot::Bump, 1, &texture->TextureSRV);
            Graphics->DeviceContext->PSSetSamplers(0, 1, &texture->SamplerState);
        }

        if (MaterialInfo.TextureSlotMask & MaterialTextureFlags::Alpha) {
            std::shared_ptr<FTexture> texture = FEngineLoop::ResourceManager.GetTexture(MaterialInfo.AlphaTexturePath);
            Graphics->DeviceContext->PSSetShaderResources(ETextureSlot::Alpha, 1, &texture->TextureSRV);
            Graphics->DeviceContext->PSSetSamplers(0, 1, &texture->SamplerState);
        }

        /*else {
            ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
            ID3D11SamplerState* nullSampler[1] = { nullptr };
            Graphics->DeviceContext->PSSetShaderResources(0, 1, nullSRV);
            Graphics->DeviceContext->PSSetSamplers(0, 1, nullSampler);
        }*/
    }
}
