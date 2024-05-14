#include "Engine/Renderer/Renderer.hpp"

struct ID3D11Buffer;
struct ID3D11Device;

class IndexBuffer
{
	friend class Renderer;

public:
	IndexBuffer(ID3D11Device* device, size_t size);
	IndexBuffer(const IndexBuffer& copy) = delete;
	
	virtual ~IndexBuffer();

	unsigned int GetStride() const;

	ID3D11Buffer* m_buffer = nullptr;
	size_t m_size = 0;

	ID3D11Buffer* CreateIndexBuffer(ID3D11Device* device, size_t size);
	void Resize(ID3D11Device* device, size_t newSize);

	unsigned int m_indexCount = 0;
};