#pragma once

#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "ThirdParty/tinyxml2/tinyxml2.h"


//---------------------------------------------------------------------------------------------------
// Generic (and shorter) nicknames for tinyxml2 types; allows us to pivot to a different
// library later if need be.
typedef tinyxml2::XMLDocument  XmlDocument;
typedef tinyxml2::XMLElement   XmlElement;
typedef tinyxml2::XMLAttribute XmlAttribute;
typedef tinyxml2::XMLError	   XmlError;


int			ParseXmlAttribute( XmlElement const& element, char const* attributeName, int defaultValue );
char		ParseXmlAttribute( XmlElement const& element, char const* attributeName, char defaultValue );
bool		ParseXmlAttribute( XmlElement const& element, char const* attributeName, bool defaultValue );
float		ParseXmlAttribute( XmlElement const& element, char const* attributeName, float defaultValue );
Rgba8		ParseXmlAttribute( XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue );
Vec2		ParseXmlAttribute( XmlElement const& element, char const* attributeName, Vec2 const& defaultValue );
IntVec2		ParseXmlAttribute( XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue );
std::string ParseXmlAttribute( XmlElement const& element, char const* attributeName, std::string const& defaultValue );
Strings		ParseXmlAttribute( XmlElement const& element, char const* attributeName, Strings const& defaultValues );
Vec3		ParseXmlAttribute( XmlElement const& element, char const* attributeName, Vec3 const& defaultValue );
EulerAngles ParseXmlAttribute( XmlElement const& element, char const* attributeName, EulerAngles const& defaultValue );
FloatRange	ParseXmlAttribute( XmlElement const& element, char const* attributeName, FloatRange const& defaultValue );

// custom special-case function for getting an attribute as a std::string, by providing the default value as tradional / hardcoded
// c-ctyle char const* text (will be important later)
std::string ParseXmlAttribute( XmlElement const& element, char const* attributeName, char const* defaultValue );

unsigned int QueryIntText( XmlElement const& element );
std::string	 ParseCData(XmlElement const& element);