#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <string_view>

typedef std::vector<std::string>	  Strings;
typedef std::vector<std::string_view> StringsView;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( char const* format, ... );
const std::string Stringf( int maxLength, char const* format, ... );


//-----------------------------------------------------------------------------------------------
Strings SplitStringOnDelimiter( std::string const& originalString, char delimiterToSplitOn );
Strings SplitStringOnCarriageReturnAndNewLine( std::string const& originalString );
Strings SplitStringOnEmptyString( std::string const& originalString );


//-----------------------------------------------------------------------------------------------
void		SplitStringsViewOnDelimiter( std::string_view const& originalString, char delimiterToSplitOn, StringsView& outSplitStringsView );
void		SplitStringViewOnSpaces( const std::string_view& originalString, StringsView& outSplitStringsView );
StringsView SplitStringOnCarriageReturnAndNewLineStringView( const std::string& originalString );