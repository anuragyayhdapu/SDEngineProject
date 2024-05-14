// Windows
#define WIN32_LEAN_AND_MEAN // Always #define this before #including <windows.h>
#include <windows.h>

// Engine
#include "Engine/Window/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

// DirectX
#include <d3d11.h> // Microsoft Direct3D 11 graphics to create 3-D graphics (https://learn.microsoft.com/en-us/windows/win32/direct3d)
#include <dxgi.h>
#include <d3dcompiler.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

// STBI Image
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

// C++
#include <system_error>


#if defined( ENGINE_DEBUG_RENDERER )
#include <dxgidebug.h>
#pragma comment( lib, "dxguid.lib" )
#endif

class Window;
extern Window* g_theWindow;

static const int k_lightingConstantSlot = 1;

struct CameraConstants
{
	Mat44 ViewMatrix;
	Mat44 ProjectionMatrix;
};
static const int k_cameraConstantSlot = 2;

struct ModelConstants
{
	Mat44 ModelMatrix;
	float ModelColor[ 4 ];
};
static const int k_modelConstantSlot = 3;


Renderer::Renderer( RendererConfig const& config )
	: m_config( config )
{
}

Renderer::~Renderer()
{
}

void Renderer::Startup()
{
	StartupDirectX11();
	SetupDxGiDebugModule();
	CreateAndBindDefaultShader();
	CreateImmediateVBO();
	CreateLightingCBO();
	CreateCameraCBO();
	CreateModelCBO();
	InitRasterizerStates();
	InitBlendStates();
	InitDefaultTexture();
	InitSamplerState();
	CreateDepthStencilView();
	InitDepthStencilStates();
}

void Renderer::StartupDirectX11()
{
	// Create the device, device context, swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferDesc.Width	   = g_theWindow->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height	   = g_theWindow->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format	   = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count	   = 1;
	swapChainDesc.BufferUsage		   = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount		   = 2;
	swapChainDesc.OutputWindow		   = ( ( HWND ) g_theWindow->GetHWND() );
	swapChainDesc.Windowed			   = true;
	swapChainDesc.SwapEffect		   = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	UINT createDeviceFlags = 0;
#if defined( ENGINE_DEBUG_RENDERER )
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_swapChain,
		&m_device, nullptr,
		&m_deviceContext );
	if ( FAILED( hr ) )
	{
		ERROR_AND_DIE( "Could not create device and swap chain." );
	}

	// Create render target view
	// Get back buffer texture
	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( void** ) &backBuffer );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not get swap chain buffer." );
	}

	hr = m_device->CreateRenderTargetView( backBuffer, NULL, &m_renderTargetView );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create render target view for swap chain buffer." );
	}

	backBuffer->Release();
}

// DirectX Debugging Tools
void Renderer::SetupDxGiDebugModule()
{
#if defined( ENGINE_DEBUG_RENDERER )

	m_dxgiDebugModule = ( void* ) ::LoadLibraryA( "dxgidebug.dll" );
	if ( m_dxgiDebugModule == nullptr )
	{
		ERROR_AND_DIE( "could not load dxgidebug.dll" );
	}

	typedef HRESULT( WINAPI * GetDebugModuleCB )( REFIID, void** );
	( ( GetDebugModuleCB )::GetProcAddress( ( HMODULE ) m_dxgiDebugModule, "DXGIGetDebugInterface" ) )( __uuidof( IDXGIDebug ), &m_dxgiDebug );

	if ( m_dxgiDebug == nullptr )
	{
		ERROR_AND_DIE( "Could not load Debug Module." );
	}

#endif
}

void Renderer::CreateAndBindDefaultShader()
{
	Shader* defaultShader = nullptr;
	// defaultShader = CreateShader("Default", DefaultSaderSource);
	defaultShader	= CreateOrGetShaderByName( "Data/Shaders/Default" );
	m_currentShader = defaultShader;
	m_defaultShader = defaultShader;

	BindShader( m_defaultShader );
}

void Renderer::CreateImmediateVBO()
{
	// create the immediate vertex and specify an initial size big enough for one Vertex_PCUTBN
	size_t		 initialSize = sizeof( Vertex_PCU );
	unsigned int stride		 = sizeof( Vertex_PCU );
	m_immediateVBO			 = new VertexBuffer( m_device, initialSize, stride );

	size_t		 initialSizePNCU = sizeof( Vertex_PCUTBN );
	unsigned int stridePNCU		 = sizeof( Vertex_PCUTBN );
	m_immediateVBOPNCU			 = new VertexBuffer( m_device, initialSizePNCU, stridePNCU );
}


IndexBuffer* Renderer::CreateAndGetIndexBuffer()
{
	IndexBuffer* indexBuffer = new IndexBuffer( m_device, sizeof( unsigned int ) );
	return indexBuffer;
}

VertexBuffer* Renderer::CreateAndGetVertexBuffer( unsigned int size, unsigned int stride )
{
	VertexBuffer* vertexBuffer = new VertexBuffer( m_device, size, stride );
	return vertexBuffer;
}

void Renderer::InitRasterizerStates()
{
	for ( int rasterizerMode = 0; rasterizerMode < ( int ) RasterizerMode::COUNT; rasterizerMode++ )
	{
		// D3D11_RASTERIZER_DESC structure defines how the rasterizer state object behaves.
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.DepthClipEnable		 = true;
		rasterizerDesc.AntialiasedLineEnable = true;

		switch ( rasterizerMode )
		{
		case ( int ) RasterizerMode::SOLID_CULL_NONE: {
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			break;
		}
		case ( int ) RasterizerMode::SOLID_CULL_BACK: {
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			break;
		}
		case ( int ) RasterizerMode::WIREFRAME_CULL_NONE: {
			rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			break;
		}
		case ( int ) RasterizerMode::WIREFRAME_CULL_BACK: {
			rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			break;
		}
		default:
			continue;
		}

		HRESULT hr;
		hr = m_device->CreateRasterizerState(
			&rasterizerDesc,
			&m_rasterizerStates[ rasterizerMode ] );
		if ( FAILED( hr ) )
		{
			std::string error = std::system_category().message( hr );
			ERROR_AND_DIE( "Could not create rasterizer state. Error:" + error );
		}
	}

	// defualt initialize to SOLID_CULL_BACK
	m_rasterizerState = m_rasterizerStates[ ( int ) RasterizerMode::SOLID_CULL_BACK ];

	m_deviceContext->RSSetState(
		m_rasterizerState );
}


void Renderer::SetRasterizerMode( RasterizerMode rasterizerMode )
{
	m_desiredRasterizerMode = rasterizerMode;
}


void Renderer::BeginFrame()
{
	SetRenderTarget();
}

void Renderer::SetRenderTarget()
{
	// set render target
	m_deviceContext->OMSetRenderTargets(
		1,
		&m_renderTargetView,
		m_depthStencilView );
}

void Renderer::EndFrame()
{
	// Present the swap chain
	HRESULT hr = m_swapChain->Present( 0, 0 );
	if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
	{
		ERROR_AND_DIE( "Device has been lost, application will now terminate." );
	}
}

void Renderer::Shutdown()
{
	ReleaseDepthStencilStates();
	ReleaseDepthStencilView();
	ReleaseSamplerState();
	DeleteDefaultTexture();
	ReleaseBlendStates();
	ReleaseRasterizerStates();
	DeleteImmediateVBO();
	DeleteModelCBO();
	DeleteCameraCBO();
	DeleteLightingCBO();
	DeleteShaderCache();
	ShutdownDirectX11();
	ReleaseImageTextures();
	ReleaseBitmapFontTextures();

	/////// call last
	ReportAndFreeDxGiDebugModule();
}

void Renderer::DeleteImmediateVBO()
{
	delete m_immediateVBO;
	delete m_immediateVBOPNCU;
}

void Renderer::DeleteLightingCBO()
{
	delete m_lightingCBO;
}

void Renderer::DeleteCameraCBO()
{
	delete m_cameraCBO;
}

void Renderer::DeleteShaderCache()
{
	for ( int index = 0; index < m_loadedShader.size(); index++ )
	{
		Shader* shader = m_loadedShader[ index ];
		delete shader;
	}
}

void Renderer::ShutdownDirectX11()
{
	// safely release memeory
	// in DirectX release the order doesn't matter, DirectX takes care of this internally
	DX_SAFE_RELEASE( m_device );
	DX_SAFE_RELEASE( m_deviceContext );
	DX_SAFE_RELEASE( m_swapChain );
	DX_SAFE_RELEASE( m_renderTargetView );
}

void Renderer::ReportAndFreeDxGiDebugModule()
{
#if defined( ENGINE_DEBUG_RENDERER )

	( ( IDXGIDebug* ) m_dxgiDebug )->ReportLiveObjects( DXGI_DEBUG_ALL, ( DXGI_DEBUG_RLO_FLAGS ) ( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL ) );

	( ( IDXGIDebug* ) m_dxgiDebug )->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary( ( HMODULE ) m_dxgiDebugModule );
	m_dxgiDebugModule = nullptr;

#endif
}

void Renderer::ReleaseBlendStates()
{
	for ( int blendMode = 0; blendMode < ( int ) BlendMode::COUNT; blendMode++ )
	{
		DX_SAFE_RELEASE( m_blendStates[ blendMode ] );
	}
}

void Renderer::ReleaseRasterizerStates()
{
	for ( int rasterizerMode = 0; rasterizerMode < ( int ) RasterizerMode::COUNT; rasterizerMode++ )
	{
		DX_SAFE_RELEASE( m_rasterizerStates[ rasterizerMode ] );
	}
}

void Renderer::ReleaseImageTextures()
{
	auto iter = m_textureRegistry.begin();
	for ( ; iter != m_textureRegistry.end(); iter++ )
	{
		Texture* loadedTexture = iter->second;
		delete loadedTexture;
	}

	m_textureRegistry.clear();
}

void Renderer::ReleaseBitmapFontTextures()
{
	for ( auto iter = m_fontRegistry.begin(); iter != m_fontRegistry.end(); iter++ )
	{
		BitmapFont* font = iter->second;
		delete font;
	}

	m_fontRegistry.clear();
}

// Clear all screen (backbuffer) pixels to medium-blue
// ALWAYS clear the screen at the top of each frame's Render()!
void Renderer::ClearScreen( const Rgba8& clearColor )
{
	float colorAsFloats[ 4 ];
	clearColor.GetAsFloats( colorAsFloats );
	m_deviceContext->ClearRenderTargetView( m_renderTargetView, colorAsFloats );

	UINT clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
	m_deviceContext->ClearDepthStencilView(
		m_depthStencilView,
		clearFlags,
		1,
		0 );
}


void Renderer::BeginCamera( const Camera& camera )
{
	CopyAndBindCameraCBO( camera );
	SetViewPort( camera.m_viewPort );

	// Set identity modal matrix
	Mat44 IdentityMatrix;
	SetModelConstants( IdentityMatrix, Rgba8::WHITE );

	// Set defualt pipeline states for respective camera
	Camera::Mode cameraMode = camera.GetMode();
	if ( cameraMode == Camera::eMode_Orthographic )
	{
		// set defualt values for Orthographic camera
		SetBlendMode( BlendMode::ALPHA );
		BindTexture( nullptr );
		BindTextures( std::vector<Texture*>{ nullptr, nullptr, nullptr } );
		BindShader( nullptr );
		SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
		SetDepthMode( DepthMode::DISABLED );
	}
	else // Camera::eMode_Perspective
	{
		// set defualt values for Perspective camera
		SetBlendMode( BlendMode::ALPHA );
		BindTexture( nullptr );
		BindShader( nullptr );
		SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
		SetDepthMode( DepthMode::ENABLED );
	}
}

void Renderer::CopyAndBindCameraCBO( const Camera& camera )
{
	CameraConstants cameraConstants;
	cameraConstants.ViewMatrix		 = camera.GetViewMatrix();
	cameraConstants.ProjectionMatrix = camera.GetProjectionMatrix();

	CopyCPUToGPU( &cameraConstants, sizeof( CameraConstants ), m_cameraCBO );
	BindConstantBuffer( k_cameraConstantSlot, m_cameraCBO );
}


// Pass model constants before the draw call of the Entity
void Renderer::SetModelConstants( const Mat44& modelMatrix, const Rgba8& modelColor )
{
	ModelConstants modelConstants;
	modelConstants.ModelMatrix = modelMatrix;
	modelColor.GetAsFloats( modelConstants.ModelColor );

	CopyCPUToGPU( &modelConstants, sizeof( ModelConstants ), m_modelCBO );
	BindConstantBuffer( k_modelConstantSlot, m_modelCBO );
}


void Renderer::SetLightingConstatnts( LightConstants const& lightingConstants )
{
	CopyCPUToGPU( &lightingConstants, sizeof( LightConstants ), m_lightingCBO );
	BindConstantBuffer( k_lightingConstantSlot, m_lightingCBO );
}


void Renderer::SetViewPort( AABB2 playerViewport )
{
	if ( playerViewport == AABB2::ZERO ) // default
	{
		// defines the size of the viewport rectangle
		D3D11_VIEWPORT d3d11ViewPort = {};
		d3d11ViewPort.TopLeftX		 = 0.f;
		d3d11ViewPort.TopLeftY		 = 0.f;
		d3d11ViewPort.Width			 = ( float ) g_theWindow->GetClientDimensions().x;
		d3d11ViewPort.Height		 = ( float ) g_theWindow->GetClientDimensions().y;
		d3d11ViewPort.MinDepth		 = 0.f;
		d3d11ViewPort.MaxDepth		 = 1.f;

		// bind viewports to the rasterizer stage
		m_deviceContext->RSSetViewports(
			1,
			&d3d11ViewPort );
	}
	else
	{
		float viewportWidth	 = playerViewport.m_maxs.x - playerViewport.m_mins.x;
		float viewportHeight = playerViewport.m_maxs.y - playerViewport.m_mins.y;

		// defines the size of the viewport rectangle
		D3D11_VIEWPORT d3d11ViewPort = {};
		d3d11ViewPort.TopLeftX		 = playerViewport.m_mins.x;
		d3d11ViewPort.TopLeftY		 = playerViewport.m_mins.y;
		d3d11ViewPort.Width			 = viewportWidth;
		d3d11ViewPort.Height		 = viewportHeight;
		d3d11ViewPort.MinDepth		 = 0.f;
		d3d11ViewPort.MaxDepth		 = 1.f;

		// bind viewports to the rasterizer stage
		m_deviceContext->RSSetViewports(
			1,
			&d3d11ViewPort );
	}
}

void Renderer::DrawVertexArray( std::vector<Vertex_PCU> const& vertexes )
{
	DrawVertexArray( ( int ) vertexes.size(), vertexes.data() );
}



// the draw function
// Draw some triangles (provide 3 vertexes each)
void Renderer::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	// copy vertex data
	size_t vertexDataSize = sizeof( Vertex_PCU ) * numVertexes;
	CopyCPUToGPU( vertexes, vertexDataSize, m_immediateVBO );

	// draw
	DrawVertexBuffer( m_immediateVBO, numVertexes, 0 );
}

void Renderer::DrawVertexArrayPCUTBN( std::vector<Vertex_PCUTBN> const& vertexes )
{
	// copy vertex data
	size_t			   numVertexes	  = vertexes.size();
	size_t			   vertexDataSize = sizeof( Vertex_PCUTBN ) * numVertexes;
	const Vertex_PCUTBN* vertexData	  = vertexes.data();

	CopyCPUToGPU( vertexData, vertexDataSize, m_immediateVBOPNCU );

	// draw
	DrawVertexBuffer( m_immediateVBOPNCU, ( int ) numVertexes, 0 );
}



void Renderer::EndCamera( const Camera& camera )
{
	UNUSED( camera );
}

RendererConfig const& Renderer::GetConfig()
{
	return m_config;
}


// Game code calls Renderer::CreateOrGetTextureFromFile(), which in turn will
//  check that name amongst the registry of already-loaded textures (by name).  If that image
//  has already been loaded, the renderer simply returns the Texture* it already has.  If the
//  image has not been loaded before, CreateTextureFromFile() gets called internally, which in
//  turn calls CreateTextureFromData.  The new Texture* is then added to the registry of
//  already-loaded textures, and then returned.
Texture* Renderer::CreateOrGetTextureFromFile( std::string imageFilePath )
{
	// See if we already have this texture previously loaded
	Texture* existingTexture = GetTextureForFileName( imageFilePath );
	if ( existingTexture )
	{
		return existingTexture;
	}

	// Never seen this texture before!  Let's load it.
	Texture* newTexture				   = CreateTextureFromFile( imageFilePath );
	m_textureRegistry[ imageFilePath ] = newTexture;
	return newTexture;
}

Texture* Renderer::GetTextureForFileName( std::string imageFilePath )
{
	if ( m_textureRegistry.find( imageFilePath ) != m_textureRegistry.cend() )
	{
		return m_textureRegistry[ imageFilePath ];
	}
	else
	{
		return nullptr;
	}
}

Texture* Renderer::CreateTextureFromFile( std::string imageFilePath )
{
	Image	 image( imageFilePath.c_str() );
	Texture* newTexture = CreateTextureFromImage( image );

	return newTexture;
}

Texture* Renderer::CreateTextureFromImage( const Image& image )
{
	// init texture description
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width				 = image.GetDimensions().x;
	textureDesc.Height				 = image.GetDimensions().y;
	textureDesc.MipLevels			 = 1;
	textureDesc.ArraySize			 = 1;
	textureDesc.Format				 = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Usage				 = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags			 = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.SampleDesc.Count	 = 1;

	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem				   = image.GetRawData();
	subResourceData.SysMemPitch			   = image.GetDimensions().x * sizeof( Rgba8 );

	// create texture
	Texture* newTexture		 = new Texture();
	newTexture->m_name		 = image.GetImageFilePath(); // NOTE: m_name must be a std::string, otherwise it may point to temporary data!
	newTexture->m_dimensions = image.GetDimensions();

	HRESULT hr;
	hr = m_device->CreateTexture2D(
		&textureDesc,
		&subResourceData,
		&( newTexture->m_texture ) );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not create texture 2d. Error:" + error );
	}

	// create shared resource view
	hr = m_device->CreateShaderResourceView(
		newTexture->m_texture,
		nullptr,
		&newTexture->m_sharedResourceView );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not create shared recource view on texture. Error:" + error );
	}

	return newTexture;
}

void Renderer::InitDefaultTexture()
{
	Image whitePixel( IntVec2( 2, 2 ), Rgba8( 255, 255, 255, 255 ) );
	m_defaultTexture = CreateTextureFromImage( whitePixel );

	BindTexture( m_defaultTexture );
}

void Renderer::InitSamplerState()
{
	for ( int samplerMode = 0; samplerMode < ( int ) SamplerMode::COUNT; samplerMode++ )
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.ComparisonFunc	   = D3D11_COMPARISON_NEVER;
		samplerDesc.MaxLOD			   = D3D11_FLOAT32_MAX;

		switch ( samplerMode )
		{
		case ( int ) SamplerMode::POINT_CLAMP: {
			samplerDesc.Filter	 = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			break;
		}
		case ( int ) SamplerMode::BILINEAR_WRAP: {
			samplerDesc.Filter	 = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			break;
		}
		default:
			continue;
		}

		HRESULT hr;
		hr = m_device->CreateSamplerState(
			&samplerDesc,
			&m_samplerStates[ ( int ) samplerMode ] );
		if ( FAILED( hr ) )
		{
			std::string error = std::system_category().message( hr );
			ERROR_AND_DIE( "Could not create sampler state. Error:" + error );
		}
	}

	// defualt initialize to point clamp
	m_samplerState = m_samplerStates[ ( int ) SamplerMode::POINT_CLAMP ];

	unsigned int vStartSlot	  = 0;
	unsigned int vNumSamplers = 1;
	m_deviceContext->VSSetSamplers(
		vStartSlot,
		vNumSamplers,
		&m_samplerState );

	unsigned int pStartSlot	  = 0;
	unsigned int pNumSamplers = 1;
	m_deviceContext->PSSetSamplers(
		pStartSlot,
		pNumSamplers,
		&m_samplerState );
}


void Renderer::SetSamplerMode( SamplerMode samplerMode )
{
	m_desiredSamplerMode = samplerMode;
}


void Renderer::DeleteDefaultTexture()
{
	delete m_defaultTexture;
}


void Renderer::BindTexture( Texture* texture )
{
	unsigned int startSlot = 0;
	unsigned int numViews  = 1;

	if ( texture == nullptr )
	{
		texture = m_defaultTexture;
	}

	m_deviceContext->PSSetShaderResources(
		startSlot,
		numViews,
		&texture->m_sharedResourceView );
}


//----------------------------------------------------------------------------------------------------------
void Renderer::BindTextures( std::vector<Texture*> const& textures )
{
	unsigned int numViews  = 1;

	for ( unsigned int index = 0; index < textures.size(); index++ )
	{
		int slot = index;
		Texture* texture = textures[ index ];
		if ( texture == nullptr )
		{
			texture = m_defaultTexture;
		}

		m_deviceContext->PSSetShaderResources(
			slot,
			numViews,
			&texture->m_sharedResourceView );
	}
}

BitmapFont* Renderer::CreateOrGetBitmapFont( const char* bitmapFontFilePathWithNoExtension )
{
	BitmapFont* bitmapFont = GetBitMapFontFromFileName( bitmapFontFilePathWithNoExtension );

	if ( bitmapFont == nullptr ) // nothing found, load font
	{
		// construct proper file name, assume .png
		std::string filePath( bitmapFontFilePathWithNoExtension );
		filePath.append( ".png" );

		Texture* fontTexture								= CreateOrGetTextureFromFile( filePath );
		bitmapFont											= new BitmapFont( bitmapFontFilePathWithNoExtension, *fontTexture );
		m_fontRegistry[ bitmapFontFilePathWithNoExtension ] = bitmapFont;
	}

	return bitmapFont;
}

BitmapFont* Renderer::GetBitMapFontFromFileName( const char* bitmapFontFilePathWithNoExtension )
{
	BitmapFont* bitmapFont = nullptr;
	auto		iter	   = m_fontRegistry.find( bitmapFontFilePathWithNoExtension );
	if ( iter != m_fontRegistry.cend() )
	{
		bitmapFont = iter->second;
	}

	return bitmapFont;
}


void Renderer::InitBlendStates()
{
	for ( int blendMode = 0; blendMode < ( int ) BlendMode::COUNT; blendMode++ )
	{
		// set blend state desc variable
		D3D11_BLEND_DESC blendDesc						  = {};
		blendDesc.RenderTarget[ 0 ].BlendEnable			  = true;
		blendDesc.RenderTarget[ 0 ].BlendOp				  = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[ 0 ].SrcBlendAlpha		  = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[ 0 ].DestBlendAlpha		  = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[ 0 ].BlendOpAlpha		  = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		switch ( blendMode )
		{
		case ( int ) BlendMode::OPAQUE: {
			blendDesc.RenderTarget[ 0 ].SrcBlend  = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ZERO;
			break;
		}
		case ( int ) BlendMode::ALPHA: {
			blendDesc.RenderTarget[ 0 ].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			break;
		}
		case ( int ) BlendMode::ADDITIVE: {
			blendDesc.RenderTarget[ 0 ].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;
			break;
		}
		default:
			continue;
		}

		// create new blend state
		HRESULT hr;
		hr = m_device->CreateBlendState(
			&blendDesc,
			&m_blendStates[ ( int ) blendMode ] );
		if ( FAILED( hr ) )
		{
			std::string error = std::system_category().message( hr );
			ERROR_AND_DIE( "Could not create blend state. Error:" + error );
		}
	}

	// default initialize to apha
	m_blendState = m_blendStates[ ( int ) BlendMode::ALPHA ];
	// set new blend state
	float		 blendFactor[ 4 ] = {};
	unsigned int sampleMask		  = 0xffffffff;
	m_deviceContext->OMSetBlendState(
		m_blendState,
		blendFactor,
		sampleMask );
}

void Renderer::SetBlendMode( BlendMode blendMode )
{
	m_desiredBlendMode = blendMode;
}


void Renderer::BindShader( Shader* shader )
{
	if ( shader == nullptr )
		shader = m_defaultShader;

	m_deviceContext->VSSetShader(
		shader->m_vertexShader,
		nullptr,
		0 );

	m_deviceContext->PSSetShader(
		shader->m_pixelShader,
		nullptr,
		0 );

	m_deviceContext->IASetInputLayout(
		shader->m_inputLayout );
}


Shader* Renderer::CreateOrGetShaderByName( char const* newShaderName, VertexType vertexType )
{
	// check if shader already exists
	for ( int index = 0; index < m_loadedShader.size(); index++ )
	{
		Shader* loadedShader = m_loadedShader[ index ];
		if ( loadedShader != nullptr )
		{
			std::string loadedShaderName = loadedShader->m_config.m_name;
			if ( loadedShaderName == newShaderName )
			{
				return loadedShader;
			}
		}
	}

	// only create new shaders, if it doesn't exist in cache
	// std::string shaderFilePath("Data/Shaders/");
	std::string shaderFilePath;
	shaderFilePath += newShaderName;
	shaderFilePath += ".hlsl";

	std::string shaderSource;
	if ( FileReadToString( shaderSource, shaderFilePath ) )
	{
		Shader* shader = CreateShaderFromShaderSource( newShaderName, shaderSource.c_str(), vertexType );
		return shader;
	}
	else
	{
		ERROR_AND_DIE( Stringf( "Unable to Load Shader form file: %s", shaderFilePath.c_str() ) );
	}
}


Shader* Renderer::CreateShaderFromShaderSource( char const* shaderName, char const* shaderSource, VertexType vertexType )
{
	// init Shader
	ShaderConfig shaderConfig;
	shaderConfig.m_name				= shaderName;
	shaderConfig.m_vertexEntryPoint = "VertexMain";
	shaderConfig.m_pixelEntrypoint	= "PixelMain";
	Shader* shader					= new Shader( shaderConfig );

	// Compile the vertex shader
	ID3DBlob*	vertexShaderByteCode = nullptr;
	char const* vertexShaderTarget	 = "vs_5_0";
	CompileShaderToByteCode(
		&vertexShaderByteCode,
		shader->GetName().c_str(),
		shaderSource,
		shader->m_config.m_vertexEntryPoint.c_str(),
		vertexShaderTarget );

	// Compile the pixel shader
	ID3DBlob*	pixelShaderByteCode = nullptr;
	char const* pixelShaderTarget	= "ps_5_0";
	CompileShaderToByteCode(
		&pixelShaderByteCode,
		shader->GetName().c_str(),
		shaderSource,
		shader->m_config.m_pixelEntrypoint.c_str(),
		pixelShaderTarget );

	// Create the vertex shader
	HRESULT hr;
	hr = m_device->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		nullptr,
		&( shader->m_vertexShader ) );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not create the vertex shader. Error:" + error );
	}

	// create the pixel shader
	hr = m_device->CreatePixelShader(
		pixelShaderByteCode->GetBufferPointer(),
		pixelShaderByteCode->GetBufferSize(),
		nullptr,
		&( shader->m_pixelShader ) );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not create the pixel shader. Error:" + error );
	}

	// Create input Layout ------------------------------------------------------------------------------------------------------
	// i. Create a local array of input element descriptions that defines the vertex layout.
	bool		isLitShader = false;
	std::string shaderSourceStr( shaderSource );
	if ( shader->GetName().find( "Lit" ) != std::string::npos ||
		 shaderSourceStr.find( "Normal" ) != std::string::npos )
	{
		isLitShader = true;
	}

	// check for unlit shader
	bool isUnlitShader = false;
	if ( shader->GetName().find( "Unlit" ) != std::string::npos )
	{
		isUnlitShader = true;
	}


	if ( isLitShader || isUnlitShader || vertexType == VertexType::Vertex_PCUTBN)
	{
		CreateShaderFromInputLayout_PNCUTBN( vertexShaderByteCode, shader );
		return shader;
	}
	else if (vertexType == VertexType::Vertex_Skeletal)
	{
		CreateShaderFromInputLayout_Skeletal( vertexShaderByteCode, shader );
		return shader;
	}
	else
	{
		CreateShaderFromInputLayout_PCU( vertexShaderByteCode, shader );
		return shader;
	}
}

bool Renderer::CompileShaderToByteCode( ID3DBlob** outByteCode, char const* name,
	char const* shaderSource, char const* entryPoint,
	char const* target )
{
	DWORD flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined( ENGINE_DEBUG_RENDERER )
	flags = D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	ID3DBlob* pErrorMessage = nullptr;

	HRESULT hr;
	hr = D3DCompile(
		shaderSource,
		strlen( shaderSource ),
		name,
		nullptr,
		nullptr,
		entryPoint,
		target,
		flags,
		0,
		outByteCode,
		&pErrorMessage );

	if ( !SUCCEEDED( hr ) )
	{
		if ( pErrorMessage )
		{
			char* errorMessage = ( char* ) pErrorMessage->GetBufferPointer();
			DebuggerPrintf( errorMessage );
			pErrorMessage->Release();
		}

		ERROR_AND_DIE( "Could not compile the shader" );
	}

	return true;
}



//----------------------------------------------------------------------------------------------------------
void Renderer::CreateShaderFromInputLayout_PCU( ID3DBlob* vertexShaderByteCode, Shader* outShader )
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// ii. Call create input layout
	HRESULT hr = m_device->CreateInputLayout(
		inputElementDesc,
		_countof( inputElementDesc ),
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		&outShader->m_inputLayout );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not create the input layout. Error:" + error );
	}

	// Add shader to cache
	m_loadedShader.push_back( outShader );
}


//----------------------------------------------------------------------------------------------------------
void Renderer::CreateShaderFromInputLayout_PNCUTBN( ID3DBlob* vertexShaderByteCode, Shader* outShader )
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// ii. Call create input layout
	HRESULT hr = m_device->CreateInputLayout(
		inputElementDesc,
		_countof( inputElementDesc ),
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		&outShader->m_inputLayout );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not create the input layout. Error:" + error );
	}

	// Add shader to cache
	m_loadedShader.push_back( outShader );
}


//----------------------------------------------------------------------------------------------------------
void Renderer::CreateShaderFromInputLayout_Skeletal( ID3DBlob* vertexShaderByteCode, Shader* outShader )
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",	  0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINTID",  0, DXGI_FORMAT_R32_UINT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// ii. Call create input layout
	HRESULT hr = m_device->CreateInputLayout(
		inputElementDesc,
		_countof( inputElementDesc ),
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		&outShader->m_inputLayout );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not create the input layout. Error:" + error );
	}

	// Add shader to cache
	m_loadedShader.push_back( outShader );
}


// Immediate Vertex Buffer -------------------------------------------

void Renderer::CopyCPUToGPU( const void* data, size_t size, VertexBuffer* vbo )
{
	vbo->Resize( m_device, size );

	// Copy the vertex buffer data from CPU to the GPU

	// A subresource for Direct3D 11 and Direct3D 12 refers to a single chunk of a complex texture resource (Direct3D 9 and earlier called them surfaces)

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	// Gets a pointer to the data contained in a subresource(pData), and denies the GPU access to that subresource
	// lock the pData (pointer to the data in the constant buffer), so that it can be written to
	HRESULT hr;
	hr = m_deviceContext->Map(
		vbo->m_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not deny the GPU access to the sub-resource while copying Vertex Buffer data. Error:" + error );
	}

	// write to constant buffer (pData)
	memcpy( mappedSubresource.pData, data, size );

	// unlock the constant buffer
	m_deviceContext->Unmap(
		vbo->m_buffer,
		0 );
}

void Renderer::DrawVertexBuffer( VertexBuffer* vbo, int vertexCount, int vertexOffset )
{
	BindVertexBuffer( vbo );


	SetStatesIfChanged();

	m_deviceContext->Draw( vertexCount, vertexOffset );
}


//----------------------------------------------------------------------------------------------------------
void Renderer::BindVertexBuffer( VertexBuffer* vbo )
{
	unsigned int	   stride = vbo->GetStride();
	const unsigned int offset = 0;

	// bind and array of vertex buffers to the input-assembler stage
	m_deviceContext->IASetVertexBuffers(
		0,
		1,
		&( vbo->m_buffer ),
		&stride,
		&offset );

	D3D_PRIMITIVE_TOPOLOGY primitiveTopology;
	switch ( vbo->m_primitiveType )
	{
	case PrimitiveType::LINES:
		primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case PrimitiveType::TRIANGLES:
		primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	default:
		primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	}

	m_deviceContext->IASetPrimitiveTopology( primitiveTopology );
}


// Immediate Index Buffer -------------------------------------------

void Renderer::CopyCPUToGPU( const void* data, size_t size, IndexBuffer* ibo )
{
	ibo->Resize( m_device, size );

	// Copy the index buffer data from CPU to the GPU
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT					 hr;
	hr = m_deviceContext->Map(
		ibo->m_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not deny the GPU access to the sub-resource while copying Index Buffer data. Error:" + error );
	}

	// write to index buffer (pData)
	memcpy( mappedSubresource.pData, data, size );

	// unlock the index buffer
	m_deviceContext->Unmap(
		ibo->m_buffer,
		0 );
}

void Renderer::BindIndexBuffer( IndexBuffer* ibo )
{
	const unsigned int offset = 0;

	// bind an index buffer to the input-assembler stage
	m_deviceContext->IASetIndexBuffer(
		ibo->m_buffer,
		DXGI_FORMAT_R32_UINT,
		offset );
}


void Renderer::DrawVertextBufferIndexed( IndexBuffer* ibo, VertexBuffer* vbo, unsigned int indexCount )
{
	// bind buffers
	BindIndexBuffer( ibo );
	BindVertexBuffer( vbo );

	// update rendering pipeline only once before the draw call
	SetStatesIfChanged();

	// draw
	unsigned int startIndexLocation = 0; // The location of the first index read by the GPU from the index buffer
	unsigned int baseVertexLocation = 0; // A value added to each index before reading a vertex from the vertex buffer
	m_deviceContext->DrawIndexed(
		indexCount,
		startIndexLocation,
		baseVertexLocation );
}


//----------------------------------------------------------------------------------------------------------
void Renderer::DrawVertextBufferIndexed( IndexBuffer* ibo, VertexBuffer* vbo )
{
	DrawVertextBufferIndexed( ibo, vbo, ibo->m_indexCount );
}


// Constant Buffer ----------------------------------------

void Renderer::CreateLightingCBO()
{
	m_lightingCBO = new ConstantBuffer( m_device, sizeof( LightConstants ) );
}


void Renderer::CreateCameraCBO()
{
	m_cameraCBO = new ConstantBuffer( m_device, sizeof( CameraConstants ) );
}


ConstantBuffer* Renderer::CreateConstantBuffer( size_t size )
{
	ConstantBuffer* newCBO = new ConstantBuffer( m_device, size );
	return newCBO;
}

void Renderer::CopyCPUToGPU( const void* data, size_t size, ConstantBuffer* cbo )
{
	// A subresource for Direct3D 11 and Direct3D 12 refers to a single chunk of a complex texture resource (Direct3D 9 and earlier called them surfaces)
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	// Gets a pointer to the data contained in a subresource(pData), and denies the GPU access to that subresource
	// lock the pData (pointer to the data in the constant buffer), so that it can be written to
	HRESULT hr;
	hr = m_deviceContext->Map(
		cbo->m_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Could not deny the GPU access to the sub-resource while copying Constant buffer data. Error:" + error );
	}

	// write to constant buffer (pData)
	memcpy( mappedSubresource.pData, data, size );

	// unlock the constant buffer
	m_deviceContext->Unmap(
		cbo->m_buffer,
		0 );
}

void Renderer::BindConstantBuffer( int slot, ConstantBuffer* cbo )
{
	unsigned int startSlot	= slot;
	unsigned int numBuffers = 1;

	m_deviceContext->VSSetConstantBuffers(
		startSlot,
		numBuffers,
		&cbo->m_buffer );

	m_deviceContext->PSSetConstantBuffers(
		startSlot,
		numBuffers,
		&cbo->m_buffer );
}


void Renderer::CreateModelCBO()
{
	m_modelCBO = new ConstantBuffer( m_device, sizeof( ModelConstants ) );
}

void Renderer::DeleteModelCBO()
{
	delete m_modelCBO;
}




void Renderer::ReleaseSamplerState()
{
	for ( int samplerMode = 0; samplerMode < ( int ) SamplerMode::COUNT; samplerMode++ )
	{
		DX_SAFE_RELEASE( m_samplerStates[ ( int ) samplerMode ] );
	}
}


void Renderer::CreateDepthStencilView()
{
	D3D11_TEXTURE2D_DESC texture2dDesc = {};
	texture2dDesc.Width				   = m_config.m_window->GetClientDimensions().x;
	texture2dDesc.Height			   = m_config.m_window->GetClientDimensions().y;
	texture2dDesc.MipLevels			   = 1;
	texture2dDesc.ArraySize			   = 1;
	texture2dDesc.Usage				   = D3D11_USAGE_DEFAULT;
	texture2dDesc.Format			   = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2dDesc.BindFlags			   = D3D11_BIND_DEPTH_STENCIL;
	texture2dDesc.SampleDesc.Count	   = 1;

	HRESULT hr;
	hr = m_device->CreateTexture2D(
		&texture2dDesc,
		nullptr,
		&m_depthStencilTexture );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Unable to create Depth Texture. Error:" + error );
	}

	hr = m_device->CreateDepthStencilView(
		m_depthStencilTexture,
		nullptr,
		&m_depthStencilView );
	if ( FAILED( hr ) )
	{
		std::string error = std::system_category().message( hr );
		ERROR_AND_DIE( "Unable to create Depth Stencil View. Error:" + error );
	}
}

void Renderer::InitDepthStencilStates()
{
	for ( int depthMode = 0; depthMode < ( int ) DepthMode::COUNT; depthMode++ )
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable			  = true;

		switch ( depthMode )
		{
		case ( int ) DepthMode::DISABLED: {
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc		= D3D11_COMPARISON_ALWAYS;
			break;
		}
		case ( int ) DepthMode::ENABLED: {
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS_EQUAL;
			break;
		}
		default:
			continue;
		}

		HRESULT hr;
		hr = m_device->CreateDepthStencilState(
			&depthStencilDesc,
			&m_depthStencilStates[ ( int ) depthMode ] );
		if ( FAILED( hr ) )
		{
			std::string error = std::system_category().message( hr );
			ERROR_AND_DIE( "Unable to create Depth Stencil State. Error:" + error );
		}
	}

	// set default depth mode ENABLED
	m_depthStencilState = m_depthStencilStates[ ( int ) DepthMode::ENABLED ];

	m_deviceContext->OMSetDepthStencilState(
		m_depthStencilState,
		0 );
}

void Renderer::SetDepthMode( DepthMode depthMode )
{
	m_desiredDepthMode = depthMode;
}

void Renderer::ReleaseDepthStencilView()
{
	DX_SAFE_RELEASE( m_depthStencilTexture );
	DX_SAFE_RELEASE( m_depthStencilView );
}

void Renderer::ReleaseDepthStencilStates()
{
	for ( int depthMode = 0; depthMode < ( int ) DepthMode::COUNT; depthMode++ )
	{
		DX_SAFE_RELEASE( m_depthStencilStates[ ( int ) depthMode ] );
	}
}


void Renderer::SetStatesIfChanged()
{
	// blend state
	if ( m_blendState != m_blendStates[ ( int ) m_desiredBlendMode ] )
	{
		m_blendState = m_blendStates[ ( int ) m_desiredBlendMode ];

		// set new blend state
		float		 blendFactor[ 4 ] = {};
		unsigned int sampleMask		  = 0xffffffff;
		m_deviceContext->OMSetBlendState(
			m_blendState,
			blendFactor,
			sampleMask );
	}

	// sampler state
	if ( m_samplerState != m_samplerStates[ ( int ) m_desiredSamplerMode ] )
	{
		m_samplerState = m_samplerStates[ ( int ) m_desiredSamplerMode ];

		unsigned int vStartSlot	  = 0;
		unsigned int vNumSamplers = 1;
		m_deviceContext->VSSetSamplers(
			vStartSlot,
			vNumSamplers,
			&m_samplerState );

		unsigned int pStartSlot	  = 0;
		unsigned int pNumSamplers = 1;
		m_deviceContext->PSSetSamplers(
			pStartSlot,
			pNumSamplers,
			&m_samplerState );
	}

	// rasterizer state
	if ( m_rasterizerState != m_rasterizerStates[ ( int ) m_desiredRasterizerMode ] )
	{
		m_rasterizerState = m_rasterizerStates[ ( int ) m_desiredRasterizerMode ];

		m_deviceContext->RSSetState(
			m_rasterizerState );
	}

	// depth state
	if ( m_depthStencilState != m_depthStencilStates[ ( int ) m_desiredDepthMode ] )
	{
		m_depthStencilState = m_depthStencilStates[ ( int ) m_desiredDepthMode ];

		m_deviceContext->OMSetDepthStencilState(
			m_depthStencilState,
			0 );
	}
}