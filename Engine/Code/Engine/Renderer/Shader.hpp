#include <string>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

struct ShaderConfig
{
	std::string m_name;
	std::string m_vertexEntryPoint = "VertexMain";
	std::string m_pixelEntrypoint = "PixelMain";
};

class Shader
{
	friend class Renderer;

private:
	Shader(const ShaderConfig& config);
	Shader(const Shader& copy) = delete;
	~Shader();

public:
	const std::string& GetName() const;

	ShaderConfig m_config;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*	m_pixelShader;
	ID3D11InputLayout*	m_inputLayout = nullptr;
};