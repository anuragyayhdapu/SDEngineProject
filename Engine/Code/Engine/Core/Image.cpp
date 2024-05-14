#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "ThirdParty/stb/stb_image.h"

Image::Image()
{
}

Image::~Image()
{
}

Image::Image(char const* imageFilePath) :
	m_imageFilePath(imageFilePath)
{
	LoadImageFromFilePath(m_imageFilePath);
}

Image::Image(IntVec2 size, Rgba8 color) :
	m_dimensions(size)
{
	int totalPixels = size.x * size.y;
	//m_rgbaTexels.resize(totalPixels);
	m_rgbaTexels.reserve(totalPixels);

	for (int pixelIndex = 0; pixelIndex < totalPixels; pixelIndex++)
	{
		m_rgbaTexels.push_back(color);
	}
}

std::string const& Image::GetImageFilePath() const
{
    return m_imageFilePath;
}

IntVec2 Image::GetDimensions() const
{
    return m_dimensions;
}

Rgba8 Image::GetTexelColor(IntVec2 const& texelCoords) const
{
	int texelIndex = (texelCoords.y * m_dimensions.y) + texelCoords.x;
	return m_rgbaTexels[texelIndex];
}

void Image::SetTexelColor(IntVec2 const& texelCoords, Rgba8 const& newColor)
{
	int texelIndex = (texelCoords.y * m_dimensions.y) + texelCoords.x;
	m_rgbaTexels[texelIndex] = newColor;
}

void const* Image::GetRawData() const
{
	return m_rgbaTexels.data();
}

void Image::LoadImageFromFilePath(std::string imageFilePath)
{
	int imageWidth = 0;  // This will be filled in for us indicate image width
	int imageHeight = 0; // This will be filled in for us indicate image height
	int numChannels = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	stbi_set_flip_vertically_on_load(1); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load(imageFilePath.c_str(), &imageWidth, &imageHeight, &numChannels, numComponentsRequested);
	GUARANTEE_OR_DIE(imageData, "Failed to load image: " + imageFilePath);

	m_dimensions = IntVec2(imageWidth, imageHeight);

	int totalTexels = imageWidth * imageHeight;

	// loop through the entire image data and store in our own Rgba8 format
	if (numChannels == 4)
	{
		for (int texelIndex = 0; texelIndex < totalTexels; texelIndex++)
		{
			// At 4 bytes per texel, the first three texels will be at bytes [0], [4], [8]
			int byteOffsetForThisTexel = texelIndex * numChannels;

			unsigned char r = imageData[byteOffsetForThisTexel + 0];
			unsigned char g = imageData[byteOffsetForThisTexel + 1];
			unsigned char b = imageData[byteOffsetForThisTexel + 2];
			unsigned char a = imageData[byteOffsetForThisTexel + 3];

			Rgba8 texel(r, g, b, a);
			m_rgbaTexels.push_back(texel);
		}
	}
	else if (numChannels == 3)
	{
		for (int texelIndex = 0; texelIndex < totalTexels; texelIndex++)
		{
			// At 3 bytes per texel, the first three texels will be at bytes [0], [3], [6]
			int byteOffsetForThisTexel = texelIndex * numChannels;

			unsigned char r = imageData[byteOffsetForThisTexel + 0];
			unsigned char g = imageData[byteOffsetForThisTexel + 1];
			unsigned char b = imageData[byteOffsetForThisTexel + 2];

			// hardcode alpha channel as 255 (opaque)
			unsigned char a = 255;

			Rgba8 texel(r, g, b, a);
			m_rgbaTexels.push_back(texel);
		}
	}
	else if ( numChannels == 1 )
	{
		for ( int texelIndex = 0; texelIndex < totalTexels; texelIndex++ )
		{
			// each texel has a byte

			unsigned char data = imageData[ texelIndex ];

			unsigned char r = data;
			unsigned char g = data;
			unsigned char b = data;
			unsigned char a = 255;

			Rgba8 texel( r, g, b, a );
			m_rgbaTexels.push_back( texel );
		}
	}

	// Free the raw image texel data
	stbi_image_free(imageData);
}
