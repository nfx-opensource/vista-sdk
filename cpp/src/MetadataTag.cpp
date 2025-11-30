/**
 * @file MetadataTag.cpp
 * @brief Implementation of the MetadataTag class
 */

#include <nfx/Hashing.h>
#include "dnv/vista/sdk/MetadataTag.h"

#include "dnv/vista/sdk/CodebookName.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// MetadataTag class
	//=====================================================================

	//-------------------------------------------------------------------------
	// Construction
	//-------------------------------------------------------------------------

	MetadataTag::MetadataTag( CodebookName name, const std::string& value, bool isCustom )
		: m_name{ name },
		  m_custom{ isCustom },
		  m_value{ value }
	{
		// Hash all three components for proper uniqueness
		uint32_t valueHash = nfx::hashing::hash<std::string_view, uint32_t, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS>( std::string_view{ m_value } );
		uint32_t nameHash = static_cast<uint32_t>( m_name );
		uint32_t customHash = isCustom ? 1 : 0;

		// Combine using modern hash_combine (MurmurHash3-based)
		m_hashCode = valueHash;
		m_hashCode = nfx::hashing::combine<uint32_t>( static_cast<uint32_t>( m_hashCode ), nameHash );
		m_hashCode = nfx::hashing::combine<uint32_t>( static_cast<uint32_t>( m_hashCode ), customHash );
	}

	//-------------------------------------------------------------------------
	// String conversion methods
	//-------------------------------------------------------------------------

	std::string MetadataTag::toString() const noexcept { return m_value; }

	void MetadataTag::toString( nfx::string::StringBuilder& builder, char separator ) const
	{
		const auto prefixView = CodebookNames::toPrefix( m_name );

		builder.append( prefixView );
		builder.append( m_custom
							? '~'
							: '-' );
		builder.append( m_value );
		builder.append( separator );
	}
} // namespace dnv::vista::sdk
