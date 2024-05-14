
#include "Vec4.hpp"

#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/StringUtils.hpp"


Vec4::Vec4()
{
}

Vec4::Vec4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{

}

Vec4 Vec4::operator-(Vec4 const& vecToSubtract) const
{
	return Vec4(x - vecToSubtract.x, 
		y - vecToSubtract.y, 
		z - vecToSubtract.z, 
		w - vecToSubtract.w);
}

void Vec4::operator*=(float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}


//----------------------------------------------------------------------------------------------------------
void Vec4::SetFromText( char const* text )
{
	Strings		splitStrings = SplitStringOnDelimiter( text, ',' );
	std::string xStr		 = splitStrings[ 0 ];
	std::string yStr		 = splitStrings[ 1 ];
	std::string zStr		 = splitStrings[ 2 ];
	std::string wStr		 = splitStrings[ 3 ];

	x = ( float ) atof( xStr.c_str() );
	y = ( float ) atof( yStr.c_str() );
	z = ( float ) atof( zStr.c_str() );
	w = ( float ) atof( wStr.c_str() );
}
