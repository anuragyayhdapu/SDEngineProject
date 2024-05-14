#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

Rgba8 Rgba8::BLACK   (0,   0,   0,   255);
Rgba8 Rgba8::WHITE   (255, 255, 255, 255);
Rgba8 Rgba8::RED     (255, 0,   0,   255);
Rgba8 Rgba8::SOFT_RED(128, 0,   0,   255);
Rgba8 Rgba8::GREEN   (0,   255, 0,   255);
Rgba8 Rgba8::BLUE    (0,   0,   255, 255);
Rgba8 Rgba8::YELLOW  (255, 255, 0,   255);
Rgba8 Rgba8::ORANGE  (255, 165, 0,   255);
Rgba8 Rgba8::GRAY    (128, 128, 128, 255);
Rgba8 Rgba8::CYAN    (0,   255, 255, 255);
Rgba8 Rgba8::MAGENTA (255, 0,   255, 255);
Rgba8 Rgba8::LIGHT_GREY(230, 230, 230, 255);
Rgba8 Rgba8::DARK_GREY(200, 200, 200, 255);

Rgba8 Rgba8::PALE_BLUE(174, 223, 247, 255);
Rgba8 Rgba8::SOFT_GREEN(0, 128, 0, 255);
Rgba8 Rgba8::LAVENDER(220, 204, 231, 255);
Rgba8 Rgba8::DUSTY_ROSE(216, 173, 161, 255);
Rgba8 Rgba8::MISTY_GRAY(209, 210, 214, 255);
Rgba8 Rgba8::LIGHT_CORAL(244, 194, 194, 255);


Rgba8::Rgba8() :
	r(255), g(255), b(255), a(255)
{
}

Rgba8::Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
	r(r), g(g), b(b), a(a)
{
}

void Rgba8::SetFromText(char const* text)
{
	Strings splitStrings = SplitStringOnDelimiter(text, ',');
	std::string rStr = splitStrings[0];
	std::string gStr = splitStrings[1];
	std::string bStr = splitStrings[2];

	r = (unsigned char)atoi(rStr.c_str());
	g = (unsigned char)atoi(gStr.c_str());
	b = (unsigned char)atoi(bStr.c_str());

	if (splitStrings.size() == 4)
	{
		std::string aStr = splitStrings[3];
		a = (unsigned char)atoi(aStr.c_str());
	}
}

void Rgba8::GetAsFloats(float* colorAsFloats) const
{
	colorAsFloats[0] = NormalizeByte(r);
	colorAsFloats[1] = NormalizeByte(g);
	colorAsFloats[2] = NormalizeByte(b);
	colorAsFloats[3] = NormalizeByte(a);
}


//----------------------------------------------------------------------------------------------------------
Rgba8 Rgba8::GetFromFloats( float r, float g, float b, float a )
{
	Rgba8 byteColor;
	byteColor.r = DenormalizeByte( r );
	byteColor.g = DenormalizeByte( g );
	byteColor.b = DenormalizeByte( b );
	byteColor.a = DenormalizeByte( a );

	return byteColor;
}


Rgba8 Rgba8::GetLerpColor(float fraction, Rgba8 startColor, Rgba8 endColor)
{	
	Rgba8 output;
	output.r = (unsigned char)RangeMapClamped(fraction, 0.f, 1.f, startColor.r, endColor.r);
	output.g = (unsigned char)RangeMapClamped(fraction, 0.f, 1.f, startColor.g, endColor.g);
	output.b = (unsigned char)RangeMapClamped(fraction, 0.f, 1.f, startColor.b, endColor.b);
	output.a = (unsigned char)RangeMapClamped(fraction, 0.f, 1.f, startColor.a, endColor.a);

	return output;
}


//----------------------------------------------------------------------------------------------------------
bool Rgba8::operator==( const Rgba8& compare ) const
{
	if (r == compare.r &&
		g == compare.g &&
		b == compare.b &&
		a == compare.a)
	{
		return true;
	}

	return false;
}
