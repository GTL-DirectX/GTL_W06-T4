#pragma once
#include "D3D11RHI/GraphicDevice.h"


struct FTexture
{
    FTexture(ID3D11ShaderResourceView* SRV, ID3D11Texture2D* Texture2D, ID3D11SamplerState* Sampler, FWString Name, DXGI_FORMAT TextureFormat, uint32 NewWidth, uint32 NewHeight)
        : TextureSRV(SRV), Texture(Texture2D), SamplerState(Sampler), Name(Name), TextureFormat(TextureFormat), Width(NewWidth), Height(NewHeight)
    {}
    ~FTexture() = default;

    void Release()
    {
        if (TextureSRV)
        {
            TextureSRV->Release();
            TextureSRV = nullptr;
        }
        if (Texture)
        {
            Texture->Release();
            Texture = nullptr;
        }
        if (SamplerState)
        {
            SamplerState->Release();
            SamplerState = nullptr;
        }
    }
    FWString Name;
    ID3D11ShaderResourceView* TextureSRV = nullptr;
    ID3D11Texture2D* Texture = nullptr;
    ID3D11SamplerState* SamplerState = nullptr;
    DXGI_FORMAT TextureFormat;
    uint32 Width;
    uint32 Height;
};
