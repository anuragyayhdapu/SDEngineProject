#pragma once
class Rgba8
{
public:
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

	Rgba8();
	explicit Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

	void SetFromText(char const* text);	// Parses “127,255,100” or “127,255,100,127” to Rgba
	void GetAsFloats(float* colorAsFloats) const;
	static Rgba8 GetFromFloats( float r, float g, float b, float a = 1.f );
	static Rgba8 GetLerpColor(float fraction, Rgba8 startColor, Rgba8 endColor);

	static Rgba8 BLACK;
	static Rgba8 WHITE;
	static Rgba8 RED;
	static Rgba8 SOFT_RED;
	static Rgba8 GREEN;
	static Rgba8 BLUE;
	static Rgba8 YELLOW;
	static Rgba8 ORANGE;
	static Rgba8 GRAY;
	static Rgba8 CYAN;
	static Rgba8 MAGENTA;
	static Rgba8 LIGHT_GREY;
	static Rgba8 DARK_GREY;

	static Rgba8 PALE_BLUE;
	static Rgba8 SOFT_GREEN;
	static Rgba8 LAVENDER;
	static Rgba8 DUSTY_ROSE;
	static Rgba8 MISTY_GRAY;
	static Rgba8 LIGHT_CORAL;

	bool		 operator==( const Rgba8& compare ) const;
};

