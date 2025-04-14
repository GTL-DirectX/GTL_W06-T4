#pragma once
#include <memory>
#include "Texture.h"
#include "Container/Map.h"

class FRenderer;
class FGraphicsDevice;
class FResourceMgr
{

public:
    void Initialize(FRenderer* renderer, FGraphicsDevice* device);
    void Release(FRenderer* renderer);
    HRESULT LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename, DXGI_FORMAT textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
    HRESULT LoadTextureFromDDS(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename, DXGI_FORMAT textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

    std::shared_ptr<FTexture> GetTexture(const FWString& name) const;
private:
    TMap<FWString, std::shared_ptr<FTexture>> textureMap;
};
