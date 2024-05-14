#include "Engine/Core/HashedCaseInsensitiveString.hpp"


//----------------------------------------------------------------------------------------------------------
HashedCaseInsensitiveString::HashedCaseInsensitiveString( HashedCaseInsensitiveString const& other )
	: m_caseIntactText( other.m_caseIntactText ), m_lowerCaseHash( other.m_lowerCaseHash )
{
}

//----------------------------------------------------------------------------------------------------------
HashedCaseInsensitiveString::HashedCaseInsensitiveString( char const* originalText )
	: m_caseIntactText( originalText ), m_lowerCaseHash( CalculateHashFromText( originalText ) )
{
}


//----------------------------------------------------------------------------------------------------------
HashedCaseInsensitiveString::HashedCaseInsensitiveString( std::string const& originalText )
	: m_caseIntactText( originalText ), m_lowerCaseHash( CalculateHashFromText( originalText ) )
{
}


//----------------------------------------------------------------------------------------------------------
unsigned int HashedCaseInsensitiveString::GetHah() const
{
	return m_lowerCaseHash;
}


//----------------------------------------------------------------------------------------------------------
std::string const& HashedCaseInsensitiveString::GetOriginalString() const
{
	return m_caseIntactText;
}


//----------------------------------------------------------------------------------------------------------
char const* HashedCaseInsensitiveString::c_str() const
{
	return m_caseIntactText.c_str();
}


//----------------------------------------------------------------------------------------------------------
unsigned int HashedCaseInsensitiveString::CalculateHashFromText( char const* text )
{
	unsigned int hash = 0;
	for ( char const* scan = text; *scan != '\0'; ++scan )
	{
		hash *= 31;
		hash += ( unsigned int ) towlower( *scan );
	}

	return hash;
}


//----------------------------------------------------------------------------------------------------------
unsigned int HashedCaseInsensitiveString::CalculateHashFromText( std::string const& text )
{
	return CalculateHashFromText( text.c_str() );
}


//----------------------------------------------------------------------------------------------------------
bool HashedCaseInsensitiveString::operator<( HashedCaseInsensitiveString const& compareHCIS ) const
{
	if ( m_lowerCaseHash < compareHCIS.m_lowerCaseHash )
	{
		return true;
	}
	else if ( m_lowerCaseHash > compareHCIS.m_lowerCaseHash )
	{
		return false;
	}
	else
	{
		return _stricmp( m_caseIntactText.c_str(), compareHCIS.m_caseIntactText.c_str() ) < 0;
	}
}


//----------------------------------------------------------------------------------------------------------
bool HashedCaseInsensitiveString::operator>( HashedCaseInsensitiveString const& compareHCIS ) const
{
	if ( m_lowerCaseHash > compareHCIS.m_lowerCaseHash )
	{
		return true;
	}
	else if ( m_lowerCaseHash < compareHCIS.m_lowerCaseHash )
	{
		return false;
	}
	else
	{
		return _stricmp( m_caseIntactText.c_str(), compareHCIS.m_caseIntactText.c_str() ) > 0;
	}
}


//----------------------------------------------------------------------------------------------------------
bool HashedCaseInsensitiveString::operator==( HashedCaseInsensitiveString const& compareHCIS ) const
{
	if ( m_lowerCaseHash != compareHCIS.m_lowerCaseHash )
	{
		return false;
	}
	else
	{
		// hash equal, do actual string comparison
		return _stricmp( m_caseIntactText.c_str(), compareHCIS.m_caseIntactText.c_str() ) == 0;
	}
}


//----------------------------------------------------------------------------------------------------------
bool HashedCaseInsensitiveString::operator!=( HashedCaseInsensitiveString const& compareHCIS ) const
{
	if ( m_lowerCaseHash != compareHCIS.m_lowerCaseHash )
	{
		return true;
	}
	else
	{
		// hash equal, do actual string comparison
		return _stricmp( m_caseIntactText.c_str(), compareHCIS.m_caseIntactText.c_str() ) != 0;
	}
}


//----------------------------------------------------------------------------------------------------------
void HashedCaseInsensitiveString::operator=( HashedCaseInsensitiveString const& assignFromHCIS )
{
	m_caseIntactText = assignFromHCIS.m_caseIntactText;
	m_lowerCaseHash	 = assignFromHCIS.m_lowerCaseHash;
}


//----------------------------------------------------------------------------------------------------------
bool HashedCaseInsensitiveString::operator==( std::string const& compareStringText ) const
{
	return _stricmp( m_caseIntactText.c_str(), compareStringText.c_str() ) == 0;
}


//----------------------------------------------------------------------------------------------------------
bool HashedCaseInsensitiveString::operator!=( std::string const& compareStringText ) const
{
	return _stricmp( m_caseIntactText.c_str(), compareStringText.c_str() ) != 0;
}


//----------------------------------------------------------------------------------------------------------
void HashedCaseInsensitiveString::operator=( std::string const& assignFromStringText )
{
	m_caseIntactText = assignFromStringText;
	m_lowerCaseHash	 = CalculateHashFromText( m_caseIntactText );
}


//----------------------------------------------------------------------------------------------------------
bool HashedCaseInsensitiveString::operator==( char const* compareText ) const
{
	return _stricmp( m_caseIntactText.c_str(), compareText ) == 0;
}


//----------------------------------------------------------------------------------------------------------
bool HashedCaseInsensitiveString::operator!=( char const* compareText ) const
{
	return _stricmp( m_caseIntactText.c_str(), compareText ) != 0;
}


//----------------------------------------------------------------------------------------------------------
void HashedCaseInsensitiveString::operator=( char const* assignFromText )
{
	m_caseIntactText = assignFromText;
	m_lowerCaseHash	 = CalculateHashFromText( m_caseIntactText );
}
