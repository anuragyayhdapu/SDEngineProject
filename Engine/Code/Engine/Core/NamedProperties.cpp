#include "Engine/Core/NamedProperties.hpp"


//----------------------------------------------------------------------------------------------------------
void NamedProperties::SetValue( std::string const& keyName, char const* value )
{
	HSCIString key( keyName );
	m_keyValuePairs[ key ] = new NamedPropertyOfType<std::string>( value );
}


//----------------------------------------------------------------------------------------------------------
std::string NamedProperties::GetValue( std::string const& keyName, char const* defaultValue )
{
	HSCIString										   key( keyName );
	std::map<HSCIString, NamedPropertyBase*>::iterator found = m_keyValuePairs.find( key );
	if ( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}

	NamedPropertyBase*		property	  = found->second;
	NamedPropertyOfType<std::string>* typedProperty = dynamic_cast<NamedPropertyOfType<std::string>*>( property );
	if ( typedProperty == nullptr )
	{
		ERROR_RECOVERABLE( "Error: Incorrect type value asked" );
		return defaultValue;
	}

	return typedProperty->m_value;
}


//----------------------------------------------------------------------------------------------------------
void NamedProperties::SetValue( std::string const& keyName, std::string const& value )
{
	HSCIString key( keyName );
	m_keyValuePairs[ key ] = new NamedPropertyOfType<std::string>( value );
}


//----------------------------------------------------------------------------------------------------------
std::string NamedProperties::GetValue( std::string const& keyName, std::string const& defaultValue )
{
	HSCIString										   key( keyName );
	std::map<HSCIString, NamedPropertyBase*>::iterator found = m_keyValuePairs.find( key );
	if ( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}

	NamedPropertyBase*				  property		= found->second;
	NamedPropertyOfType<std::string>* typedProperty = dynamic_cast<NamedPropertyOfType<std::string>*>( property );
	if ( typedProperty == nullptr )
	{
		ERROR_RECOVERABLE( "Error: Incorrect type value asked" );
		return defaultValue;
	}

	return typedProperty->m_value;
}
