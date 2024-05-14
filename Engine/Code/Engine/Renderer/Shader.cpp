// Engine
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Renderer.hpp"

// DirectX
#include <d3d11.h>


Shader::Shader(const ShaderConfig& config)
{
	m_config.m_name				= config.m_name;
	m_config.m_vertexEntryPoint = config.m_vertexEntryPoint;
	m_config.m_pixelEntrypoint	= config.m_pixelEntrypoint;
}

Shader::~Shader()
{
	DX_SAFE_RELEASE(m_vertexShader);
	DX_SAFE_RELEASE(m_pixelShader);
	DX_SAFE_RELEASE(m_inputLayout);
}

const std::string& Shader::GetName() const
{
	return m_config.m_name;
}
