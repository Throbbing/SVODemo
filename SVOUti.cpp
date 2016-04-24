#include"SVOUti.h"


void jmxRCore::updateCBTrans(ID3D11Buffer* buf, CXMMATRIX w, ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	HR(dc->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	CBTrans* t = (CBTrans*)ms.pData;
	XMStoreFloat4x4(&t->world, XMMatrixTranspose(w));
	dc->Unmap(buf, 0);
}

void jmxRCore::updateCBInfo(ID3D11Buffer* buf, u32 cur, u32 total,u32 sumv,u32 cn, ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	HR(dc->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	CBInfo* t = (CBInfo*)ms.pData; 
	t->curLevel = cur;
	t->totalLevel = total;
	t->sumVoxels = sumv;
	t->curNode = cn;
	dc->Unmap(buf, 0);
}

void jmxRCore::updateCBProj(ID3D11Buffer* buf, CXMMATRIX vp0, CXMMATRIX vp1, CXMMATRIX vp2, 
	 ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	HR(dc->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	CBProj* t = (CBProj*)ms.pData;
	XMStoreFloat4x4(&t->vproj[0], XMMatrixTranspose(vp0));
	XMStoreFloat4x4(&t->vproj[1], XMMatrixTranspose(vp1));
	XMStoreFloat4x4(&t->vproj[2], XMMatrixTranspose(vp2));
	dc->Unmap(buf, 0);
}

void jmxRCore::updateCBAttri(ID3D11Buffer* buf, Vec3 e, Vec3 o, ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	HR(dc->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	CBAttri* t = (CBAttri*)ms.pData;
	t->extent = Vec4(e,0.f);
	t->origin = Vec4(o,0.f);
	dc->Unmap(buf, 0);
}

void jmxRCore::updateCBBrickInfo(ID3D11Buffer* buf, u32 d[3], ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	HR(dc->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	CBBrickInfo* t = (CBBrickInfo*)ms.pData;
	for (u32 i = 0; i < 3; ++i)
		t->extent[i] = d[i];
	dc->Unmap(buf, 0);
}

void jmxRCore::updateCBGroupInfo(ID3D11Buffer* buf, u32 d[3], ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	HR(dc->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	CBGroupInfo* t = (CBGroupInfo*)ms.pData;
	for (u32 i = 0; i < 3; ++i)
		t->groupSize[i] = d[i];
	dc->Unmap(buf, 0);
}

jmxRCore::AtomicIndex::AtomicIndex(ID3D11Device* dv,ID3D11DeviceContext* ct,
	s32* data)
{
	this->device = dv;
	this->context = ct;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bd.ByteWidth = 4;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
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

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	ZeroMemory(&uavd, sizeof(uavd));
	uavd.Format = DXGI_FORMAT_R32_SINT;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavd.Buffer.FirstElement = 0;
	uavd.Buffer.NumElements = 1;
	uavd.Buffer.Flags = 0;
	HR(device->CreateUnorderedAccessView(buf, &uavd, &uav));

	

	bd.Usage = D3D11_USAGE_STAGING;
	bd.BindFlags = 0;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	HR(device->CreateBuffer(&bd, 0, &staging));




}

s32 jmxRCore::AtomicIndex::getValue()
{
	context->CopyResource(staging, buf);

	D3D11_MAPPED_SUBRESOURCE ms;
	HR(context->Map(staging, 0, D3D11_MAP_READ, 0, &ms));

	s32* v = (s32*)ms.pData;
	s32 vv = *v;
	context->Unmap(staging, 0);

	return vv;
}

jmxRCore::AtomicIndexArray::AtomicIndexArray(ID3D11Device* dv, ID3D11DeviceContext* ct,
	u32 totalCount,s32* data)
{
	this->device = dv;
	this->context = ct;
	this->count = totalCount;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bd.ByteWidth = sizeof(s32)*count;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
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

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	ZeroMemory(&uavd, sizeof(uavd));
	uavd.Format = DXGI_FORMAT_R32_SINT;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavd.Buffer.FirstElement = 0;
	uavd.Buffer.NumElements = count;
	uavd.Buffer.Flags = 0;
	HR(device->CreateUnorderedAccessView(buf, &uavd, &uav));



	bd.Usage = D3D11_USAGE_STAGING;
	bd.BindFlags = 0;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	HR(device->CreateBuffer(&bd, 0, &staging));
}

std::vector<s32> jmxRCore::AtomicIndexArray::getValue()
{
	context->CopyResource(staging, buf);

	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(staging, 0, D3D11_MAP_READ, 0, &ms);

	s32* v = (s32*)ms.pData;
	std::vector<s32> vec;
	for (u32 i = 0; i < count; ++i)
		vec.push_back(v[i]);
	context->Unmap(staging, 0);

	return vec;
}