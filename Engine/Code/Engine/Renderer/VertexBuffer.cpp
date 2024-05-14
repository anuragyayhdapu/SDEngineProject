#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <d3d11.h>
#include <system_error>

VertexBuffer::VertexBuffer(ID3D11Device* device,  size_t size, unsigned int stride) :
	m_size(size), m_stride(stride)
{
	m_buffer = CreateVertexBuffer(device, m_size);
}

VertexBuffer::~VertexBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}

unsigned int VertexBuffer::GetStride() const
{
	//return sizeof(Vertex_PCUTBN);
	return m_stride;
}


void VertexBuffer::Resize(ID3D11Device* device, size_t newSize)
{
	if (m_size < newSize)
	{
		DX_SAFE_RELEASE(m_buffer);
		m_buffer = CreateVertexBuffer(device, newSize);
		m_size = newSize;
	}
}


ID3D11Buffer* VertexBuffer::CreateVertexBuffer(ID3D11Device* device, size_t size)
{
	ID3D11Buffer* buffer = nullptr;

	// i. Create a local buffer description variable
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = (UINT)size;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// ii. Create buffer
	HRESULT hr;
	hr = device->CreateBuffer
	(
		&vertexBufferDesc,
		nullptr,
		&(buffer)
	);
	if (FAILED(hr))
	{
		std::string error = std::system_category().message(hr);
		ERROR_AND_DIE("Could not create vertex buffer. Error:" + error);
	}

	return buffer;
}