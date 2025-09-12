/**
 * @file MetadataTag.cpp
 * @brief Implementation of the MetadataTag class
 */

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
		  m_value{ value } {}

	//-------------------------------------------------------------------------
	// String conversion methods
	//-------------------------------------------------------------------------

	std::string MetadataTag::toString() const noexcept { return m_value; }

	void MetadataTag::toString( nfx::string::StringBuilder& builder, char separator ) const
	{
		const auto prefixView = CodebookNames::toPrefix( m_name );

		builder.append( prefixView );
		builder.push_back( m_custom
							   ? '~'
							   : '-' );
		builder.append( m_value );
		builder.push_back( separator );
	}
}
