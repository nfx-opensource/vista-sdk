/**
 * @file DataChannelId.cpp
 * @brief Implementation of DataChannelId class
 */

#include <nfx/string/StringBuilderPool.h>
#include <nfx/string/Utils.h>

#include "dnv/vista/sdk/transport/timeseries/DataChannelId.h"

#include "dnv/vista/sdk/LocalIdBuilder.h"

namespace dnv::vista::sdk::transport::timeseries
{
	//=====================================================================
	// DataChannelId class implementation
	//=====================================================================

	//----------------------------------------------
	// String conversion
	//----------------------------------------------

	std::string DataChannelId::toString() const
	{
		switch ( m_tag )
		{
			case Tag::LocalId:
			{
				return m_localId->toString();
			}
			case Tag::ShortId:
			{
				return *m_shortId;
			}
			default:
			{
				throw std::runtime_error{ "Invalid DataChannelId state: corrupted tag " + std::to_string( static_cast<int>( m_tag ) ) };
			}
		}
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	DataChannelId DataChannelId::parse( std::string_view value )
	{
		if ( nfx::string::isEmpty( value ) )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "DataChannelId::parse: value cannot be empty" );

			throw std::invalid_argument{ lease.toString() };
		}

		// Try to parse as LocalId first
		std::optional<LocalId> localId;
		if ( LocalId::tryParse( value, localId ) )
		{
			return DataChannelId{ *localId };
		}

		// If not a valid LocalId, treat as short identifier
		return DataChannelId{ value };
	}
}
