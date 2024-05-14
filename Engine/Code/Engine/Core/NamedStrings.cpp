#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"


void NamedStrings::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	XmlAttribute const* attribute = element.FirstAttribute();
	while (attribute != nullptr)
	{
		std::string name = attribute->Name();
		std::string value = attribute->Value();
		SetValue(name, value);

		attribute = attribute->Next();
	}
}

void NamedStrings::SetValue(std::string const& keyName, std::string const& newValue)
{
	m_keyValuePairs[keyName] = newValue;
}

std::string NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	std::string strValue(defaultValue);
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		strValue = iter->second;
	}

	return strValue;
}

bool NamedStrings::GetValue(std::string const& keyName, bool defaultValue) const
{
	bool boolValue = defaultValue;
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		std::string boolStr = iter->second;
		if (boolStr == "true" || boolStr == "True" || boolStr == "TRUE")
		{
			boolValue = true;
		}
		else if (boolStr == "false" || boolStr == "False" || boolStr == "FALSE")
		{
			boolValue = false;
		}
	}

	return boolValue;
}

int NamedStrings::GetValue(std::string const& keyName, int defaultValue) const
{
	int intValue = defaultValue;
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		std::string intStr = iter->second;
		intValue = atoi(intStr.c_str());
	}

	return intValue;
}

float NamedStrings::GetValue(std::string const& keyName, float defaultValue) const
{
	float floatValue = defaultValue;
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		std::string intStr = iter->second;
		floatValue = (float)atof(intStr.c_str());
	}

	return floatValue;
}

std::string NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const
{
	std::string strValue(defaultValue);
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		strValue = iter->second;
	}

	return strValue;
}

Rgba8 NamedStrings::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const
{
	Rgba8 rgbValue = defaultValue;
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		std::string rgbStr = iter->second;
		rgbValue.SetFromText(rgbStr.c_str());
	}

	return rgbValue;
}

Vec2 NamedStrings::GetValue(std::string const& keyName, Vec2 const& defaultValue) const
{
	Vec2 vec2Value = defaultValue;
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		std::string rgbStr = iter->second;
		vec2Value.SetFromText(rgbStr.c_str());
	}

	return vec2Value;
}

IntVec2 NamedStrings::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const
{
	IntVec2 intVec2Value = defaultValue;
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		std::string rgbStr = iter->second;
		intVec2Value.SetFromText(rgbStr.c_str());
	}

	return intVec2Value;
}

Vec3 NamedStrings::GetValue(std::string const& keyName, Vec3 const& defaultValue) const
{
	Vec3 vec3Value = defaultValue;
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		std::string rgbStr = iter->second;
		vec3Value.SetFromText(rgbStr.c_str());
	}

	return vec3Value;
}

EulerAngles NamedStrings::GetValue(std::string const& keyName, EulerAngles const& defaultValue) const
{
	EulerAngles eulerAngleValue = defaultValue;
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		std::string rgbStr = iter->second;
		eulerAngleValue.SetFromText(rgbStr.c_str());
	}

	return eulerAngleValue;
}


//----------------------------------------------------------------------------------------------------------
Vec4 NamedStrings::GetValue( std::string const& keyName, Vec4 const& defaultValue ) const
{
	Vec4 vec4Value = defaultValue;
	auto iter	   = m_keyValuePairs.find( keyName );
	if ( iter != m_keyValuePairs.cend() )
	{
		std::string rgbStr = iter->second;
		vec4Value.SetFromText( rgbStr.c_str() );
	}

	return vec4Value;
}


bool NamedStrings::HasKey(std::string const& keyName) const
{
	auto iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.cend())
	{
		return true;
	}

	return false;
}
