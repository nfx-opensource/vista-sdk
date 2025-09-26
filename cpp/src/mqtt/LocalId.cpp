/**
 * @file LocalId.cpp
 * @brief MQTT-compatible LocalId implementation
 */

#include "dnv/vista/sdk/mqtt/LocalId.h"

#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/MetadataTag.h"
#include "dnv/vista/sdk/VISVersion.h"
#include "dnv/vista/sdk/transport/ISO19848Constants.h"

#include <nfx/string/StringBuilderPool.h>

namespace dnv::vista::sdk::mqtt
{
	namespace internal
	{
		//----------------------------------------------
		// MQTT formatting constants and helpers
		//----------------------------------------------

		/** @brief Internal separator for MQTT paths */
		static constexpr char INTERNAL_SEPARATOR = '_';

		/**
		 * @brief Appends GMOD path to builder with MQTT formatting.
		 * @param[in,out] builder StringBuilder to append to.
		 * @param[in] path GmodPath to append with underscore separators.
		 */
		static void appendPath( nfx::string::StringBuilder& builder, const GmodPath& path )
		{
			// Convert path to string with MQTT separator (underscore instead of slash)
			path.toString( builder, INTERNAL_SEPARATOR );
			builder.append( "/" );
		}

		/**
		 * @brief Appends primary item to builder in MQTT format.
		 * @param[in] localIdBuilder LocalIdBuilder containing the primary item.
		 * @param[in,out] builder StringBuilder to append to.
		 */
		static void appendPrimaryItem( const LocalIdBuilder& localIdBuilder, nfx::string::StringBuilder& builder )
		{
			appendPath( builder, localIdBuilder.primaryItem().value() );
		}

		/**
		 * @brief Appends secondary item or placeholder to builder in MQTT format.
		 * @param[in] localIdBuilder LocalIdBuilder containing the optional secondary item.
		 * @param[in,out] builder StringBuilder to append to.
		 */
		static void appendSecondaryItem( const LocalIdBuilder& localIdBuilder, nfx::string::StringBuilder& builder )
		{
			if ( localIdBuilder.secondaryItem().has_value() )
			{
				appendPath( builder, localIdBuilder.secondaryItem().value() );
			}
			else
			{
				// No secondary item - use placeholder
				builder.append( "_/" );
			}
		}

		/**
		 * @brief Appends metadata tag or placeholder to builder in MQTT format.
		 * @details Appends the metadata tag value if present, otherwise appends an underscore placeholder.
		 *          Always appends a trailing forward slash for MQTT topic formatting.
		 * @param[in,out] builder StringBuilder to append to.
		 * @param[in] tag Optional metadata tag to append (or placeholder if empty).
		 */
		static void appendMeta( nfx::string::StringBuilder& builder, const std::optional<MetadataTag>& tag )
		{
			if ( !tag.has_value() )
			{
				builder.append( "_/" );
			}
			else
			{
				tag.value().toString( builder );
			}
		}
	}

	//=====================================================================
	// LocalId class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	LocalId::LocalId( sdk::LocalIdBuilder builder )
		: sdk::LocalId( std::move( builder ) )
	{
	}

	//----------------------------------------------
	// MQTT-specific string conversion
	//----------------------------------------------

	std::string LocalId::toString() const
	{
		auto lease = nfx::string::StringBuilderPool::lease();
		auto builder = lease.builder();

		// Naming rule (NO leading slash for MQTT): "dnv-v2/"
		builder.append( transport::ISO19848_ANNEX_C_NAMING_RULE );
		builder.append( "/" );

		builder.append( VisVersionExtensions::toVersionString( visVersion() ) );
		builder.append( "/" );

		internal::appendPrimaryItem( m_builder, builder );

		internal::appendSecondaryItem( m_builder, builder );

		internal::appendMeta( builder, quantity() );
		internal::appendMeta( builder, content() );
		internal::appendMeta( builder, calculation() );
		internal::appendMeta( builder, state() );
		internal::appendMeta( builder, command() );
		internal::appendMeta( builder, type() );
		internal::appendMeta( builder, position() );
		internal::appendMeta( builder, detail() );

		// 6. Remove trailing slash if present
		if ( builder.length() > 0 && builder[builder.length() - 1] == '/' )
		{
			builder.resize( builder.length() - 1 );
		}

		return lease.toString();
	}
}
