#include"D3DUti.h"
#include<d3dCompiler.h>
void* jmxRCore::createShaderAndLayout(LPCTSTR srcFile,
	const D3D10_SHADER_MACRO * macro,
	LPD3D10INCLUDE include,
	LPCSTR name, LPCSTR sm, EShader es,
	ID3D11Device* device,
	const D3D11_INPUT_ELEMENT_DESC* desc,
	u32 descNum,
	ID3D11InputLayout** layout)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
//	dwShaderFlags |= D3DCOMPILE_DEBUG;
//	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* blob = nullptr;
	ID3DBlob* err = nullptr;
	HR(D3DX11CompileFromFile(srcFile, macro, include, name, sm, dwShaderFlags,
		0, 0, &blob, &err,nullptr));

	/*
	if (err)
	{
		char* msg = (char*)err->GetBufferPointer();
		std::cout << msg << std::endl;
		ReleaseCom(err);

		return nullptr;
	}
	*/
	if (desc&&descNum > 0)
	{
		HR(device->CreateInputLayout(desc, descNum,
			blob->GetBufferPointer(), blob->GetBufferSize(), layout));

	}

	switch (es)
	{
	case jmxRCore::EVs:
		ID3D11VertexShader* vs;
		HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vs));
		return (void*)vs;
		break;
	case jmxRCore::EGs:
		ID3D11GeometryShader* gs;
		HR(device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &gs));
		return (void*)gs;
		break;
	case jmxRCore::EHs:
		ID3D11HullShader* hs;
		HR(device->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &hs));
		return (void*)hs;
		break;
	case jmxRCore::EDs:
		ID3D11DomainShader* ds;
		HR(device->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &ds));
		return (void*)ds;
		break;
	case jmxRCore::ECs:
		ID3D11ComputeShader* cs;
		HR(device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &cs));
		return (void*)cs;
		break;
	case jmxRCore::EPs:
		ID3D11PixelShader* ps;
		HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &ps));
		return (void*)ps;
		break;
	default:return nullptr;
		break;
	}

}

ID3D11Buffer* jmxRCore::createConstantBuffer(u32 byteWidth,ID3D11Device* device,
	D3D11_USAGE usage /* = D3D11_USAGE_DYNAMIC */,
	D3D11_CPU_ACCESS_FLAG cpuFlag /* = D3D11_CPU_ACCESS_WRITE */)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = byteWidth;
	bd.CPUAccessFlags = cpuFlag;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	
	ID3D11Buffer* buf;
	HR(device->CreateBuffer(&bd, 0, &buf));

	return buf;
}

ID3D11ShaderResourceView* jmxRCore::createSRV(ID3D11Resource* src, DXGI_FORMAT format,
	D3D11_SRV_DIMENSION dim,
	u32 arg0, u32 arg1, u32 arg2,
	ID3D11Device* device)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = dim;

	switch (dim)
	{
	case D3D11_SRV_DIMENSION_BUFFER:
		srvd.Buffer.FirstElement = 0;
		srvd.Buffer.NumElements = arg0;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE2D:
		srvd.Texture2D.MipLevels = arg0;
		srvd.Texture2D.MostDetailedMip = arg1;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE3D:
		srvd.Texture3D.MipLevels = arg0;
		srvd.Texture3D.MostDetailedMip = arg1;
		break;
	case D3D11_SRV_DIMENSION_BUFFEREX:
		srvd.BufferEx.FirstElement = 0;
		srvd.BufferEx.NumElements = arg0;
		srvd.BufferEx.Flags = arg1;
		break;
	default:return nullptr;
		break;
	}
	ID3D11ShaderResourceView* srv;
	HR(device->CreateShaderResourceView(src, &srvd, &srv));

	return srv;
	
	
}

ID3D11UnorderedAccessView* jmxRCore::createUAV(ID3D11Resource* src,
	DXGI_FORMAT format, D3D11_UAV_DIMENSION dim,
	u32 arg0, u32 arg1, u32 arg2,
	ID3D11Device* device)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	ZeroMemory(&uavd, sizeof(uavd));
	uavd.Format = format;
	uavd.ViewDimension = dim;
	switch (dim)
	{

	case D3D11_UAV_DIMENSION_BUFFER:
		uavd.Buffer.FirstElement = 0;
		uavd.Buffer.NumElements = arg0;
		uavd.Buffer.Flags = arg1;
		break;
	case D3D11_UAV_DIMENSION_TEXTURE2D:
		uavd.Texture2D.MipSlice = 0;
		break;
	case D3D11_UAV_DIMENSION_TEXTURE3D:
		uavd.Texture3D.MipSlice = 0;
		uavd.Texture3D.FirstWSlice = 0;
		uavd.Texture3D.WSize = arg0;
		break;
	default:return nullptr;
		break;
	}

	ID3D11UnorderedAccessView* uav;
	HR(device->CreateUnorderedAccessView(src, &uavd, &uav));

	return uav;
}

ID3D11Buffer* jmxRCore::createBuffer(D3D11_USAGE usage, u32 bindflag,
	u32 byteWidth,
	u32 misc,
	u32 cpuflag,
	u32 stride, void* data,
	ID3D11Device* device)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.BindFlags = bindflag;
	bd.ByteWidth = byteWidth;
	bd.MiscFlags = misc;
	bd.CPUAccessFlags = cpuflag;
	bd.StructureByteStride = stride;

	ID3D11Buffer* buf = nullptr;
	D3D11_SUBRESOURCE_DATA sd;
	if (data)
	{
		sd.pSysMem = data;
		HR(device->CreateBuffer(&bd, &sd, &buf));
	}
	else
	{
		HR(device->CreateBuffer(&bd, 0, &buf));
	}

	return buf;

}