#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
constexpr int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( char const* format, ... )
{
	char	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( int maxLength, char const* format, ... )
{
	char  textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if ( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if ( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}


//-----------------------------------------------------------------------------------------------
Strings SplitStringOnDelimiter( const std::string& originalString, char delimiterToSplitOn )
{
	Strings splitStrings;

	size_t pos = 0, lastPos = 0;
	while ( ( pos = originalString.find( delimiterToSplitOn, lastPos ) ) != std::string::npos )
	{
		std::string&& subString = originalString.substr( lastPos, pos - lastPos );
		if ( !subString.empty() )
		{
			splitStrings.push_back( originalString.substr( lastPos, pos - lastPos ) );
		}

		lastPos = pos + 1;
	}

	std::string&& remainingPart = originalString.substr( lastPos );
	if (!remainingPart.empty())
	{
		splitStrings.push_back( remainingPart );
	}

	return splitStrings;
}


//-----------------------------------------------------------------------------------------------
Strings SplitStringOnCarriageReturnAndNewLine( const std::string& originalString )
{
	Strings splitStrings;

	size_t pos = 0, lastPos = 0;
	while ( ( pos = originalString.find( "\r\n", lastPos ) ) != std::string::npos )
	{
		splitStrings.push_back( originalString.substr( lastPos, pos - lastPos ) );
		lastPos = pos + 2;
	}

	while ( ( pos = originalString.find( '\n', lastPos ) ) != std::string::npos )
	{
		splitStrings.push_back( originalString.substr( lastPos, pos - lastPos ) );
		lastPos = pos + 1;
	}

	// Push the remaining part
	if ( lastPos <= originalString.size() )
	{
		splitStrings.push_back( originalString.substr( lastPos ) );
	}

	return splitStrings;
}


//-----------------------------------------------------------------------------------------------
Strings SplitStringOnEmptyString( const std::string& originalString )
{
	Strings splitStrings;

	size_t pos = 0, lastPos = 0;
	while ( ( pos = originalString.find( ' ', lastPos ) ) != std::string::npos )
	{
		if ( pos != lastPos )
		{
			splitStrings.push_back( originalString.substr( lastPos, pos - lastPos ) );
		}
		lastPos = pos + 1;
	}

	// Push the remaining part if it's not empty
	if ( lastPos < originalString.size() )
	{
		splitStrings.push_back( originalString.substr( lastPos ) );
	}

	return splitStrings;
}



//-----------------------------------------------------------------------------------------------
void SplitStringsViewOnDelimiter( const std::string_view& originalString, char delimiterToSplitOn, StringsView& outSplitStringsView )
{
	outSplitStringsView.clear();

	size_t pos = 0, lastPos = 0;
	while ( ( pos = originalString.find( delimiterToSplitOn, lastPos ) ) != std::string::npos )
	{
		outSplitStringsView.emplace_back( originalString.data() + lastPos, pos - lastPos );
		lastPos = pos + 1;
	}

	outSplitStringsView.emplace_back( originalString.data() + lastPos, originalString.size() - lastPos );
}


//-----------------------------------------------------------------------------------------------
void SplitStringViewOnSpaces( const std::string_view& originalString, StringsView& outSplitStringsView )
{
	outSplitStringsView.clear();

	size_t pos = 0, lastPos = 0;
	while ( ( pos = originalString.find( ' ', lastPos ) ) != std::string::npos )
	{
		if ( pos != lastPos )
		{
			outSplitStringsView.emplace_back( originalString.data() + lastPos, pos - lastPos );
		}

		// Skip over consecutive spaces
		while ( pos < originalString.size() && originalString[ pos ] == ' ' )
			pos++;

		lastPos = pos;
	}

	// Push the remaining part if it's not empty
	if ( lastPos < originalString.size() )
	{
		outSplitStringsView.emplace_back( originalString.data() + lastPos, originalString.size() - lastPos );
	}
}


//-----------------------------------------------------------------------------------------------
StringsView SplitStringOnCarriageReturnAndNewLineStringView( const std::string& originalString )
{
	StringsView splitStrings;

	size_t pos = 0, lastPos = 0;
	while ( lastPos < originalString.size() )
	{
		pos = originalString.find_first_of( "\r\n", lastPos );
		if ( pos == std::string::npos )
		{
			// Push the last part and break
			std::string_view lineView( originalString.data() + lastPos, originalString.size() - lastPos );
			if ( !lineView.empty() )
				splitStrings.emplace_back( lineView );
			break;
		}

		std::string_view lineView( originalString.data() + lastPos, pos - lastPos );
		if ( !lineView.empty() )
			splitStrings.emplace_back( lineView );
		lastPos = ( originalString[ pos ] == '\r' && pos + 1 < originalString.size() && originalString[ pos + 1 ] == '\n' ) ? pos + 2 : pos + 1;
	}

	return splitStrings;
}