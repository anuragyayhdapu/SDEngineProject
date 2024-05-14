#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <d3d11.h>
#include <system_error>

ConstantBuffer::ConstantBuffer(ID3D11Device* device, size_t size)
    : m_size(size)
{
    m_buffer = CreateConstantBuffer(device, m_size);
}

ConstantBuffer::~ConstantBuffer()
{
    DX_SAFE_RELEASE(m_buffer);
}

ID3D11Buffer* ConstantBuffer::CreateConstantBuffer(ID3D11Device* device, const size_t size)
{
    ID3D11Buffer* buffer = nullptr;

	// i. Create a local buffer description variable
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = (UINT)size;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// ii. Create buffer
	HRESULT hr;
	hr = device->CreateBuffer
	(
		&constantBufferDesc,
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
