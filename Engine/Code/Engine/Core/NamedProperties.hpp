#pragma once

#include "Engine/Core/HashedCaseInsensitiveString.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <map>
#include <string>


//----------------------------------------------------------------------------------------------------------
class NamedPropertyBase
{
public:
	NamedPropertyBase() {}
	virtual ~NamedPropertyBase() {}
};


//----------------------------------------------------------------------------------------------------------
template <typename T>
class NamedPropertyOfType : public NamedPropertyBase
{
	friend class NamedProperties;

protected:
	NamedPropertyOfType( T value ) : m_value( value ) {}
	T m_value;
};


//----------------------------------------------------------------------------------------------------------
class NamedProperties
{
public:
	// std::string key name, c++ style
	template <typename T>
	void SetValue( std::string const& keyName, T const& value );

	template <typename T>
	T			GetValue( std::string const& keyName, T const& defaultValue );

	void		SetValue( std::string const& keyName, char const* value );
	std::string GetValue( std::string const& keyName, char const* defaultValue );

	void		SetValue( std::string const& keyName, std::string const& value );
	std::string GetValue( std::string const& keyName, std::string const& defaultValue );

private:
	std::map<HSCIString, NamedPropertyBase*> m_keyValuePairs;
};


//----------------------------------------------------------------------------------------------------------
template <typename T>
inline void NamedProperties::SetValue( std::string const& keyName, T const& value )
{
	HSCIString key( keyName );
	m_keyValuePairs[ key ] = new NamedPropertyOfType<T>( value );
}


//----------------------------------------------------------------------------------------------------------
template <typename T>
inline T NamedProperties::GetValue( std::string const& keyName, T const& defaultValue )
{
	HSCIString										   key( keyName );
	std::map<HSCIString, NamedPropertyBase*>::iterator found = m_keyValuePairs.find( key );
	if ( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}

	NamedPropertyBase*		property	  = found->second;
	NamedPropertyOfType<T>* typedProperty = dynamic_cast<NamedPropertyOfType<T>*>( property );
	if ( typedProperty == nullptr )
	{
		ERROR_RECOVERABLE( "Error: Incorrect type value asked" );
		return defaultValue;
	}

	return typedProperty->m_value;
}
