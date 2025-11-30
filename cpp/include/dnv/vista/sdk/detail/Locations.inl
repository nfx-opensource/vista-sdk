/**
 * @file Locations.inl
 * @brief Inline implementations for performance-critical Locations operations
 */

#include <charconv>

#include <nfx/string/Utils.h>

namespace dnv::vista::sdk
{
	//=====================================================================
	// Location Class
	//=====================================================================

	//----------------------------------------------
	// Comparison Operators
	//----------------------------------------------

	inline bool Location::operator==( const Location& other ) const
	{
		return nfx::string::equals( m_value, other.m_value );
	}

	inline bool Location::operator!=( const Location& other ) const
	{
		return !nfx::string::equals( m_value, other.m_value );
	}

	//----------------------------------------------
	// Conversion Operators
	//----------------------------------------------

	inline Location::operator std::string() const noexcept
	{
		return m_value;
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	const inline std::string& Location::value() const noexcept
	{
		return m_value;
	}

	const inline std::string& Location::toString() const noexcept
	{
		return m_value;
	}

	inline std::size_t Location::hashCode() const noexcept
	{
		return m_hashCode;
	}

	//=====================================================================
	// RelativeLocation Class
	//=====================================================================

	//----------------------------------------------
	// Comparison Operators
	//----------------------------------------------

	inline bool RelativeLocation::operator==( const RelativeLocation& other ) const
	{
		return m_code == other.m_code;
	}

	inline bool RelativeLocation::operator!=( const RelativeLocation& other ) const
	{
		return !( m_code == other.m_code );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline char RelativeLocation::code() const noexcept
	{
		return m_code;
	}

	inline const std::string& RelativeLocation::name() const noexcept
	{
		return m_name;
	}

	inline const std::optional<std::string>& RelativeLocation::definition() const noexcept
	{
		return m_definition;
	}

	inline const Location& RelativeLocation::location() const noexcept
	{
		return m_location;
	}
} // namespace dnv::vista::sdk
