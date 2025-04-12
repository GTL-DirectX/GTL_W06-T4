#include "DXDShaderManager.h"
#include <Define.h>


FDXDShaderManager::FDXDShaderManager(ID3D11Device* Device)
    : DXDDevice(Device)
{
    VertexShaders.Empty();
    PixelShaders.Empty();
}

void FDXDShaderManager::ReleaseAllShader()
{
    for (auto& [Key, Value] : VertexShaders)
    {
        if (Value.VertexShader)
        {
            Value.VertexShader->Release();
            Value.VertexShader = nullptr;
        }
    }
    VertexShaders.Empty();

    for (auto& [Key, Value] : PixelShaders)
    {
        if (Value.PixelShader)
        {
            Value.PixelShader->Release();
            Value.PixelShader = nullptr;
        }
    }
    PixelShaders.Empty();

}

HRESULT FDXDShaderManager::AddPixelShader(const std::wstring& Key, const std::wstring& FileName, const std::string& EntryPoint)
{
    UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    HRESULT hr = S_OK;

    if (DXDDevice == nullptr)
        return S_FALSE;

    ID3DBlob* PsBlob = nullptr;
    hr = D3DCompileFromFile(FileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryPoint.c_str(), "ps_5_0", shaderFlags, 0, &PsBlob, nullptr);
    if (FAILED(hr))
        return hr;

    ID3D11PixelShader* NewPixelShader;
    hr = DXDDevice->CreatePixelShader(PsBlob->GetBufferPointer(), PsBlob->GetBufferSize(), nullptr, &NewPixelShader);
    if (PsBlob)
    {
        PsBlob->Release();
    }
    if (FAILED(hr))
        return hr;

    FPixelShaderInfo NewPixelShaderInfo;
    NewPixelShaderInfo.FileName = FileName;
    NewPixelShaderInfo.EntryPoint = EntryPoint;
    NewPixelShaderInfo.PixelShader = NewPixelShader;

    PixelShaders[Key] = NewPixelShaderInfo;

    if (!ShaderFileTimeMap.Contains(FileName))
    {
        ShaderFileTimeMap.Add(FileName, std::filesystem::last_write_time(FileName));
    }

    return S_OK;
}

HRESULT FDXDShaderManager::AddVertexShader(const std::wstring& Key, const std::wstring& FileName)
{
    return E_NOTIMPL;
}

HRESULT FDXDShaderManager::AddVertexShader(const std::wstring& Key, const std::wstring& FileName, const std::string& EntryPoint)
{
    if (DXDDevice == nullptr)
        return S_FALSE;

    HRESULT hr = S_OK;

    ID3DBlob* VertexShaderCSO = nullptr;
    ID3DBlob* ErrorBlob = nullptr;

    hr = D3DCompileFromFile(FileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryPoint.c_str(), "vs_5_0", 0, 0, &VertexShaderCSO, &ErrorBlob);
    if (FAILED(hr))
    {
        if (ErrorBlob) {
            OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
            ErrorBlob->Release();
        }
        return hr;
    }

    ID3D11VertexShader* NewVertexShader;
    hr = DXDDevice->CreateVertexShader(VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), nullptr, &NewVertexShader);
    if (FAILED(hr))
    {
        VertexShaderCSO->Release();
        return hr;
    }

    FVertexShaderInfo NewVertexShaderInfo;
    NewVertexShaderInfo.FileName = FileName;
    NewVertexShaderInfo.EntryPoint = EntryPoint;
    NewVertexShaderInfo.VertexShader = NewVertexShader;
    NewVertexShaderInfo.Layout = nullptr;
    NewVertexShaderInfo.LayoutSize = 0;

    VertexShaders[Key] = NewVertexShaderInfo;

    if (!ShaderFileTimeMap.Contains(FileName))
    {
        ShaderFileTimeMap.Add(FileName, std::filesystem::last_write_time(FileName));
    }

    VertexShaderCSO->Release();

    return S_OK;
}

HRESULT FDXDShaderManager::AddInputLayout(const std::wstring& Key, const D3D11_INPUT_ELEMENT_DESC* Layout, uint32_t LayoutSize)
{
    return S_OK;
}

HRESULT FDXDShaderManager::AddVertexShaderAndInputLayout(const std::wstring& Key, const std::wstring& FileName, const std::string& EntryPoint, D3D11_INPUT_ELEMENT_DESC* Layout, uint32_t LayoutSize)
{
    UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    if (DXDDevice == nullptr)
        return S_FALSE;

    HRESULT hr = S_OK;

    ID3DBlob* VertexShaderCSO = nullptr;
    ID3DBlob* ErrorBlob = nullptr;

    hr = D3DCompileFromFile(FileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryPoint.c_str(), "vs_5_0", shaderFlags, 0, &VertexShaderCSO, &ErrorBlob);
    if (FAILED(hr))
    {
        if (ErrorBlob) {
            OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
            ErrorBlob->Release();
        }
        return hr;
    }

    ID3D11VertexShader* NewVertexShader;
    hr = DXDDevice->CreateVertexShader(VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), nullptr, &NewVertexShader);
    if (FAILED(hr))
    {
        return hr;
    }

    ID3D11InputLayout* NewInputLayout;
    hr = DXDDevice->CreateInputLayout(Layout, LayoutSize, VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), &NewInputLayout);
    if (FAILED(hr))
    {
        VertexShaderCSO->Release();
        return hr;
    }

    FVertexShaderInfo NewVertexShaderInfo;
    NewVertexShaderInfo.FileName = FileName;
    NewVertexShaderInfo.EntryPoint = EntryPoint;
    NewVertexShaderInfo.VertexShader = NewVertexShader;
    NewVertexShaderInfo.Layout = Layout;
    NewVertexShaderInfo.LayoutSize = LayoutSize;

    VertexShaders[Key] = NewVertexShaderInfo;
    InputLayouts[Key] = NewInputLayout;

    if (!ShaderFileTimeMap.Contains(FileName))
    {
        ShaderFileTimeMap.Add(FileName, std::filesystem::last_write_time(FileName));
    }

    VertexShaderCSO->Release();

    return S_OK;
}

ID3D11InputLayout* FDXDShaderManager::GetInputLayoutByKey(const std::wstring& Key) const
{
    if (InputLayouts.Contains(Key))
    {
        return *InputLayouts.Find(Key);
    }
    return nullptr;
}

ID3D11VertexShader* FDXDShaderManager::GetVertexShaderByKey(const std::wstring& Key) const
{
    if (VertexShaders.Contains(Key))
    {
        return VertexShaders.Find(Key)->VertexShader;
    }
    return nullptr;
}

ID3D11PixelShader* FDXDShaderManager::GetPixelShaderByKey(const std::wstring& Key) const
{
    if (PixelShaders.Contains(Key))
    {
        return PixelShaders.Find(Key)->PixelShader;
    }
    return nullptr;
}

void FDXDShaderManager::HotReloadShader()
{
    // ShaderFile 업데이트 여부 확인.

    for (auto& [Key, Value] : VertexShaders)
    {
        if (Value.VertexShader && IsOutDated(Value.FileName))
        {
            UE_LOG(LogLevel::Display, TEXT("Shader Hot Reload"));
            Value.VertexShader->Release();
            AddVertexShaderAndInputLayout(Key, Value.FileName, Value.EntryPoint, Value.Layout, Value.LayoutSize);
            UpdateShaderFileTime(Key);
        }
    }

    for (auto& [Key, Value] : PixelShaders)
    {
        if (Value.PixelShader && IsOutDated(Value.FileName))
        {
            Value.PixelShader->Release();
            AddPixelShader(Key, Value.FileName, Value.EntryPoint);
            UpdateShaderFileTime(Key);
        }
    }

}

bool FDXDShaderManager::IsOutDated(const std::wstring& FileName)
{
    std::filesystem::file_time_type CurrentTime = std::filesystem::last_write_time(FileName);

    if (ShaderFileTimeMap.Contains(FileName))
    {
        if (CurrentTime != ShaderFileTimeMap[FileName])
        {
            ShaderFileTimeMap[FileName] = CurrentTime;
            return true;
        }
    }
    return false;
}

void FDXDShaderManager::UpdateShaderFileTime(const std::wstring& FileName)
{
    std::filesystem::file_time_type CurrentTime = std::filesystem::last_write_time(FileName);
    ShaderFileTimeMap[FileName] = CurrentTime;
}
