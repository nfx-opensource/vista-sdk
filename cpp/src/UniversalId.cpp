/**
 * @file UniversalId.cpp
 * @brief Implementation of UniversalId class
 */

#include <nfx/string/StringBuilderPool.h>

#include "dnv/vista/sdk/UniversalId.h"

#include "dnv/vista/sdk/ImoNumber.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/UniversalIdBuilder.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// UniversalId class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	UniversalId::UniversalId( const UniversalIdBuilder& builder )
		: m_imoNumber{ *builder.imoNumber() },
		  m_localId{ builder.localId()->build() }
	{
		if ( !builder.isValid() )
		{
			throw std::invalid_argument{ "Invalid UniversalIdBuilder state" };
		}
	}

	//----------------------------------------------
	// String conversion
	//----------------------------------------------

	std::string UniversalId::toString() const
	{
		auto lease = nfx::string::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder.append( transport::ISO19848_ANNEX_C_NAMING_ENTITY );
		builder.append( "/" );
		builder.append( m_imoNumber.toString() );
		builder.append( m_localId.toString() );

		return lease.toString();
	}

	//----------------------------------------------
	// Static parsing methods
	//----------------------------------------------

	UniversalId UniversalId::parse( std::string_view universalIdStr )
	{
		auto builder = UniversalIdBuilder::parse( universalIdStr );

		return builder.build();
	}

	bool UniversalId::tryParse( std::string_view universalIdStr, ParsingErrors& errors, std::optional<UniversalId>& universalId )
	{
		std::optional<UniversalIdBuilder> builder;

		if ( !UniversalIdBuilder::tryParse( universalIdStr, errors, builder ) )
		{
			universalId = std::nullopt;
			return false;
		}

		universalId = builder->build();

		return true;
	}
}
