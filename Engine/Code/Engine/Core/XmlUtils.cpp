#include "Engine/Core/XmlUtils.hpp"

int ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue)
{
	int val = element.IntAttribute(attributeName, defaultValue);
	return val;
}

char ParseXmlAttribute( XmlElement const& element, char const* attributeName, char defaultValue )
{
	const char* val = element.Attribute( attributeName );
	if ( val == nullptr )
	{
		return defaultValue;
	}
	return *val;
}

bool ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue)
{
	bool val = element.BoolAttribute(attributeName, defaultValue);
	return val;
}

float ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue)
{
	float val = element.FloatAttribute(attributeName, defaultValue);
	return val;
}

Rgba8 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue)
{
	Rgba8 rgb8(defaultValue);
	const char* rgb8Str = element.Attribute(attributeName);
	if (rgb8Str)
	{
		rgb8.SetFromText(rgb8Str);
	}
	return rgb8;
}

Vec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue)
{
	Vec2 vec2(defaultValue);
	const char* vec2Str = element.Attribute(attributeName);
	if (vec2Str)
	{
		vec2.SetFromText(vec2Str);
	}
	return vec2;
}

IntVec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue)
{
	IntVec2 intVec2(defaultValue);
	const char* intVec2Str = element.Attribute(attributeName);
	if (intVec2Str)
	{
		intVec2.SetFromText(intVec2Str);
	}
	return intVec2;
}

std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue)
{
	std::string str = defaultValue;
	const char* xmlParsedValue = element.Attribute(attributeName);
	if (xmlParsedValue)
	{
		str = xmlParsedValue;
	}
	return str;
}

Strings ParseXmlAttribute(XmlElement const& element, char const* attributeName, Strings const& defaultValues)
{
	Strings strings(defaultValues);
	const char* str = element.Attribute(attributeName);
	if (str)
	{
		strings.push_back(str);
	}
	return strings;
}

Vec3 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec3 const& defaultValue)
{
	Vec3 vec3(defaultValue);
	const char* vec3Str = element.Attribute(attributeName);
	if ( vec3Str )
	{
		vec3.SetFromText(vec3Str);
	}
	return vec3;
}

EulerAngles ParseXmlAttribute(XmlElement const& element, char const* attributeName, EulerAngles const& defaultValue)
{
	EulerAngles eulerAngle(defaultValue);
	const char* eulerAngleStr = element.Attribute(attributeName);
	if ( eulerAngleStr )
	{
		eulerAngle.SetFromText(eulerAngleStr);
	}
	return eulerAngle;
}

FloatRange ParseXmlAttribute(XmlElement const& element, char const* attributeName, FloatRange const& defaultValue)
{
	FloatRange floatRange(defaultValue);
	const char* floatRangeStr = element.Attribute(attributeName);
	if ( floatRangeStr )
	{
		floatRange.SetFromText(floatRangeStr);
	}
	return floatRange;
}

std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue)
{
	std::string str = defaultValue;
	const char* xmlParsedValue = element.Attribute(attributeName);
	if (xmlParsedValue)
	{
		str = xmlParsedValue;
	}
	return str;
}

unsigned int QueryIntText(XmlElement const& element)
{
	unsigned int data;
	element.QueryUnsignedText(&data);

	return data;
}


//----------------------------------------------------------------------------------------------------------
std::string ParseCData( XmlElement const& element )
{
	tinyxml2::XMLNode const* cdataNode	  = element.FirstChild();
	char const*				 cdataContent = cdataNode->Value();

	return std::string( cdataContent );
}
