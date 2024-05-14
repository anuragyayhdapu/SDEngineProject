#pragma once

#include "Game/EngineBuildPreferences.hpp"

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <vector>
#include <string>
#include <map>

#ifdef OPAQUE
#undef OPAQUE
#endif // OPAQUE

enum class BlendMode
{
	OPAQUE,
	ALPHA,
	ADDITIVE,
	COUNT
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	COUNT
};

enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
};

enum class DepthMode
{
	DISABLED,
	ENABLED,
	COUNT
};


class Window;
class Texture;
class BitmapFont;
class Shader;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;


// DirectX 11
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11RasterizerState;
struct ID3D10Blob; // for shader byte code
typedef ID3D10Blob ID3DBlob;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11Texture2D;


// Safely release a Direct3D object--------------
#define DX_SAFE_RELEASE(dxObject)				\
{												\
	if ((dxObject) != nullptr)					\
	{											\
		(dxObject)->Release();					\
		(dxObject) = nullptr;					\
	}											\
}	
// ----------------------------------------------


struct RendererConfig
{
	Window* m_window;
};


class Renderer
{
public:
	Renderer(RendererConfig const& config);
	~Renderer();
	RendererConfig const& GetConfig();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void ClearScreen(const Rgba8& clearColor);
	void BeginCamera(const Camera& camera);
	void EndCamera(const Camera& camera);

	void DrawVertexArray(std::vector<Vertex_PCU> const& vertexes);
	void DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes);

	void DrawVertexArrayPCUTBN(std::vector<Vertex_PCUTBN> const& vertexes);
	
	Texture* CreateOrGetTextureFromFile(std::string	imageFilepath);
	BitmapFont* CreateOrGetBitmapFont(const char* bitmapFontFilePathWithNoExtension);
	void BindTexture(Texture* texture);
	void BindTextures( std::vector<Texture*> const& textures );

	void SetStatesIfChanged();
	void SetBlendMode(BlendMode blendMode);
	void SetSamplerMode(SamplerMode samplerMode);
	void SetRasterizerMode(RasterizerMode rasterizerMode);
	void SetDepthMode(DepthMode depthMode);

	void SetModelConstants(const Mat44& modelMatrix = Mat44(), const Rgba8& modelColor = Rgba8::WHITE);
	void SetLightingConstatnts( LightConstants const& lightingConstants );

	Shader* CreateOrGetShaderByName(char const* shaderName, VertexType vertexType = VertexType::Vertex_PCU);
	void	BindShader(Shader* shader);
	void CopyCPUToGPU(const void* data, size_t size, IndexBuffer* ibo); // index buffer
	void CopyCPUToGPU(const void* data, size_t size, VertexBuffer* vbo); // vertex buffer

	void DrawVertextBufferIndexed(IndexBuffer* ibo, VertexBuffer* vbo, unsigned int indexCount);
	void DrawVertextBufferIndexed(IndexBuffer* ibo, VertexBuffer* vbo);

	IndexBuffer* CreateAndGetIndexBuffer();
	VertexBuffer* CreateAndGetVertexBuffer(unsigned int size, unsigned int stride);

	void DrawVertexBuffer(VertexBuffer* vbo, int vertexCount, int vertexOffset = 0);

	// Constant Buffer
	ConstantBuffer* CreateConstantBuffer(size_t size);
	void CopyCPUToGPU(const void* data, size_t size, ConstantBuffer* cbo);
	void BindConstantBuffer(int slot, ConstantBuffer* cbo);

protected:
	RendererConfig m_config;

	// Texture
	Texture* m_defaultTexture = nullptr;
	std::map<std::string, Texture*> m_textureRegistry;
	void InitDefaultTexture();
	Texture* CreateTextureFromFile(std::string imageFilePath);
	Texture* CreateTextureFromImage(const Image& image);
	Texture* GetTextureForFileName(std::string imageFilePath);
	void DeleteDefaultTexture();
	void ReleaseImageTextures();
	
	// Font Texture
	std::map<std::string, BitmapFont*> m_fontRegistry;
	BitmapFont* GetBitMapFontFromFileName(const char* bitmapFontFilePathWithNoExtension);
	void ReleaseBitmapFontTextures();
	
	// DirectX11
	ID3D11Device*				m_device			= nullptr;
	ID3D11DeviceContext*		m_deviceContext		= nullptr;
	IDXGISwapChain*				m_swapChain			= nullptr;		// The IDXGISwapChain stores the buffers that are used for rendering data.
	ID3D11RenderTargetView*		m_renderTargetView	= nullptr;

	// for d3d11 debugging
	void* m_dxgiDebug		= nullptr;
	void* m_dxgiDebugModule = nullptr;

	void StartupDirectX11();
	void SetupDxGiDebugModule();
	void ReportAndFreeDxGiDebugModule();
	void ShutdownDirectX11();

	// Shader cache
	std::vector<Shader*> m_loadedShader;
	Shader const*		 m_currentShader = nullptr;
	Shader*				 m_defaultShader = nullptr;

	
	
	Shader* CreateShaderFromShaderSource( char const* shaderName, char const* shaderSource, VertexType vertexType = VertexType::Vertex_PCU );
	bool	CompileShaderToByteCode(ID3DBlob** outByteCode, char const* name, char const* source, 
								    char const* entryPoint, char const* target);
	void CreateAndBindDefaultShader();
	void DeleteShaderCache();

	void	CreateShaderFromInputLayout_PCU( ID3DBlob* vertexShaderByteCode, Shader* outShader);
	void	CreateShaderFromInputLayout_PNCUTBN( ID3DBlob* vertexShaderByteCode, Shader* outShader);
	void	CreateShaderFromInputLayout_Skeletal( ID3DBlob* vertexShaderByteCode, Shader* outShader );

	// Immediate Vertex Buffer
	VertexBuffer* m_immediateVBO = nullptr;
	VertexBuffer* m_immediateVBOPNCU = nullptr;
	void CreateImmediateVBO();
	void DeleteImmediateVBO();
	
	void BindVertexBuffer(VertexBuffer* vbo);
	
	void BindIndexBuffer(IndexBuffer* ibo);
	

	// Constant Buffer
	ConstantBuffer* m_lightingCBO = nullptr;
	void CreateLightingCBO();
	void DeleteLightingCBO();

	ConstantBuffer* m_cameraCBO = nullptr;
	void CreateCameraCBO();
	void DeleteCameraCBO();
	void CopyAndBindCameraCBO(const Camera& camera);
	

	// model constant buffer
	ConstantBuffer* m_modelCBO = nullptr;
	void CreateModelCBO();
	void DeleteModelCBO();

	// Rasterizer State
	ID3D11RasterizerState* m_rasterizerState							  = nullptr;
	RasterizerMode		   m_desiredRasterizerMode						  = RasterizerMode::SOLID_CULL_BACK;
	ID3D11RasterizerState* m_rasterizerStates[(int)RasterizerMode::COUNT] = { };
	void InitRasterizerStates();
	void ReleaseRasterizerStates();
	
	void SetRenderTarget();
	void SetViewPort(AABB2 playerViewport = AABB2::ZERO);

	// Blend State
	ID3D11BlendState* m_blendState							 = nullptr;
	BlendMode		  m_desiredBlendMode					 = BlendMode::ALPHA;
	ID3D11BlendState* m_blendStates[(int)(BlendMode::COUNT)] = { };
	void InitBlendStates();
	void ReleaseBlendStates();

	// Sampler State
	ID3D11SamplerState* m_samplerState							   = nullptr;
	SamplerMode			m_desiredSamplerMode					   = SamplerMode::POINT_CLAMP; // WTF does this do ?
	ID3D11SamplerState* m_samplerStates[(int)(SamplerMode::COUNT)] = { };
	void InitSamplerState();
	void ReleaseSamplerState();

	// Depth Buffers
	ID3D11DepthStencilState* m_depthStencilState						   = nullptr;
	DepthMode				 m_desiredDepthMode							   = DepthMode::ENABLED;
	ID3D11DepthStencilState* m_depthStencilStates[(int)(DepthMode::COUNT)] = { };
	void InitDepthStencilStates();
	void ReleaseDepthStencilStates();
	
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11Texture2D* m_depthStencilTexture = nullptr;
	void CreateDepthStencilView();
	void ReleaseDepthStencilView();
};

