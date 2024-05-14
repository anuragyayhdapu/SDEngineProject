#pragma once

#include <string>


//----------------------------------------------------------------------------------------------------------
class HashedCaseInsensitiveString
{
public:
	HashedCaseInsensitiveString()											= default;
	HashedCaseInsensitiveString( HashedCaseInsensitiveString const& other );
	HashedCaseInsensitiveString( char const* originalText );
	HashedCaseInsensitiveString( std::string const& originalText );

	unsigned int		GetHah() const;
	std::string const&	GetOriginalString() const;
	char const*			c_str() const;

	static unsigned int CalculateHashFromText( char const* text );
	static unsigned int CalculateHashFromText( std::string const& text );

	// operators
	bool operator<( HashedCaseInsensitiveString const& compareHCIS ) const;
	bool operator>( HashedCaseInsensitiveString const& compareHCIS ) const;
	bool operator==( HashedCaseInsensitiveString const& compareHCIS ) const;
	bool operator!=( HashedCaseInsensitiveString const& compareHCIS ) const;
	void operator=( HashedCaseInsensitiveString const& assignFromHCIS );

	bool operator==( std::string const& compareStringText ) const;
	bool operator!=( std::string const& compareStringText ) const;
	void operator=( std::string const& assignFromStringText );

	bool operator==( char const* compareText ) const;
	bool operator!=( char const* compareText ) const;
	void operator=( char const* assignFromText ) ;

private:
	std::string m_caseIntactText = "";
	unsigned int m_lowerCaseHash  = 0;
};

typedef HashedCaseInsensitiveString HSCIString;