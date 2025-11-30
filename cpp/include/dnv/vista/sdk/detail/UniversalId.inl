/**
 * @file UniversalId.inl
 * @brief Inline implementations for performance-critical UniversalId operations
 */

#include "dnv/vista/sdk/config/config.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// UniversalId class
	//=====================================================================

	//----------------------------------------------
	// Operators
	//----------------------------------------------

	inline bool UniversalId::operator==( const UniversalId& other ) const noexcept
	{
		return equals( other );
	}

	inline bool UniversalId::operator!=( const UniversalId& other ) const noexcept
	{
		return !equals( other );
	}

	inline bool UniversalId::equals( const UniversalId& other ) const noexcept
	{
		return m_builder.equals( other.m_builder );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const ImoNumber& UniversalId::imoNumber() const noexcept
	{
		return *m_builder.imoNumber();
	}

	inline const LocalId& UniversalId::localId() const noexcept
	{
		return m_localId;
	}

	//----------------------------------------------
	// Hashing
	//----------------------------------------------

	inline std::size_t UniversalId::hashCode() const noexcept
	{
		return m_builder.hashCode();
	}
} // namespace dnv::vista::sdk
