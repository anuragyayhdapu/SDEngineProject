#pragma once

#include "Engine/Renderer/Renderer.hpp"

struct ID3D11Buffer;
struct ID3D11Device;


enum class PrimitiveType
{
	//POINTS,
	LINES,
	TRIANGLES,
	//QUADS,
};


class VertexBuffer
{
	friend class Renderer;

public:
	VertexBuffer(ID3D11Device* device, size_t size, unsigned int stride);
	VertexBuffer(const VertexBuffer& copy) = delete;
	
	virtual ~VertexBuffer();

	unsigned int GetStride() const;

	ID3D11Buffer* m_buffer = nullptr;
	size_t m_size = 0;

	ID3D11Buffer* CreateVertexBuffer(ID3D11Device* device, size_t size);
	void Resize(ID3D11Device* device, size_t newSize);

protected:
	unsigned int m_stride = 0;


public:
	//----------------------------------------------------------------------------------------------------------
	PrimitiveType m_primitiveType = PrimitiveType::TRIANGLES;

	unsigned int m_vertexCount = 0;
};