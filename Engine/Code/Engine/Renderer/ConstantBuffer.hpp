#include "Engine/Renderer/Renderer.hpp"

struct ID3D11Buffer;
struct ID3D11Device;

class ConstantBuffer
{
	friend class Renderer;

public:
	ConstantBuffer(ID3D11Device* device, size_t size);
	ConstantBuffer(ConstantBuffer const& copy) = delete;
	virtual ~ConstantBuffer();

	ID3D11Buffer* m_buffer = nullptr;
	size_t m_size = 0;

protected:
	ID3D11Buffer* CreateConstantBuffer(ID3D11Device* device, const size_t size);
};