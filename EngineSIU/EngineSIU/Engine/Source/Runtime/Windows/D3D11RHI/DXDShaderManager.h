#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Container/Map.h"

#include <filesystem>


struct FVertexShaderData
{
	ID3DBlob* VertexShaderCSO;
	ID3D11VertexShader* VertexShader;
};

struct FVertexShaderInfo
{
    std::wstring FileName;
    std::string EntryPoint;
    ID3D11VertexShader* VertexShader;
    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout;
    uint32_t LayoutSize;
};

struct FPixelShaderInfo
{
    std::wstring FileName;
    std::string EntryPoint;
    ID3D11PixelShader* PixelShader;
};

class FDXDShaderManager
{
public:
	FDXDShaderManager() = default;
	FDXDShaderManager(ID3D11Device* Device);

	void ReleaseAllShader();

private:
	ID3D11Device* DXDDevice;

public:
	HRESULT AddVertexShader(const std::wstring& Key, const std::wstring& FileName);
	HRESULT AddVertexShader(const std::wstring& Key, const std::wstring& FileName, const std::string& EntryPoint);
	HRESULT AddInputLayout(const std::wstring& Key, const D3D11_INPUT_ELEMENT_DESC* Layout, uint32_t LayoutSize);

	
	HRESULT AddVertexShaderAndInputLayout(const std::wstring& Key, const std::wstring& FileName, const std::string& EntryPoint, D3D11_INPUT_ELEMENT_DESC* Layout, uint32_t LayoutSize);

	HRESULT AddPixelShader(const std::wstring& Key, const std::wstring& FileName, const std::string& EntryPoint);

	ID3D11InputLayout* GetInputLayoutByKey(const std::wstring& Key) const;
	ID3D11VertexShader* GetVertexShaderByKey(const std::wstring& Key) const;
	ID3D11PixelShader* GetPixelShaderByKey(const std::wstring& Key) const;


private:
	TMap<std::wstring, ID3D11InputLayout*> InputLayouts;
	TMap<std::wstring, FVertexShaderInfo> VertexShaders;
	TMap<std::wstring, FPixelShaderInfo> PixelShaders;

public:
    void HotReloadShader();

private:
    bool IsOutDated(const std::wstring& FileName);
    void UpdateShaderFileTime(const std::wstring& FileName);

private:
    TMap<std::wstring, std::filesystem::file_time_type> ShaderFileTimeMap;


};

