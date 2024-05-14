#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <d3d11.h>
#include <system_error>

IndexBuffer::IndexBuffer(ID3D11Device* device,  size_t size) :
	m_size(size)
{
	m_buffer = CreateIndexBuffer(device, m_size);
}

IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}

unsigned int IndexBuffer::GetStride() const
{
	return sizeof(Vertex_PCU);
}


void IndexBuffer::Resize(ID3D11Device* device, size_t newSize)
{
	if (m_size < newSize)
	{
		DX_SAFE_RELEASE(m_buffer);
		m_buffer = CreateIndexBuffer(device, newSize);
		m_size = newSize;
	}
}


ID3D11Buffer* IndexBuffer::CreateIndexBuffer(ID3D11Device* device, size_t size)
{
	ID3D11Buffer* buffer = nullptr;

	// i. Create a local buffer description variable
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = (UINT)size;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// this is different for index buffer
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// ii. Create buffer
	HRESULT hr;
	hr = device->CreateBuffer
	(
		&indexBufferDesc,
		nullptr,
		&(buffer)
	);
	if (FAILED(hr))
	{
		std::string error = std::system_category().message(hr);
		ERROR_AND_DIE("Could not create index buffer. Error:" + error);
	}

	return buffer;
}