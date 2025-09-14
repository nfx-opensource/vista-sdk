/**
 * @file DataChannelHelpers.h
 * @brief Internal helper functions for DataChannel validation and processing
 * @details Contains utility functions for validating numeric values against restrictions,
 *          boundary checking, and error message generation for DataChannel operations.
 */

#pragma once

#include <nfx/string/StringBuilderPool.h>

#include "dnv/vista/sdk/transport/datachannel/DataChannel.h"
#include "dnv/vista/sdk/Results.h"

namespace dnv::vista::sdk::transport
{
	namespace internal
	{
		//=====================================================================
		// Restriction class helpers
		//=====================================================================

		/**
		 * @brief Validate numeric value against bounds
		 * @param restriction The restriction rules to validate against
		 * @param number Numeric value to validate
		 * @return ValidateResult with success or bound violation errors
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		static ValidateResult validateNumber( const Restriction& restriction, double number )
		{
			if ( restriction.minExclusive() && number <= *restriction.minExclusive() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Value must be greater than " );
				builder.append( std::to_string( *restriction.minExclusive() ) );

				return ValidateResult::Invalid{ { lease.toString() } };
			}

			if ( restriction.maxExclusive() && number >= *restriction.maxExclusive() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Value must be less than " );
				builder.append( std::to_string( *restriction.maxExclusive() ) );

				return ValidateResult::Invalid{ { lease.toString() } };
			}

			if ( restriction.minInclusive() && number < *restriction.minInclusive() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Value must be greater than or equal to " );
				builder.append( std::to_string( *restriction.minInclusive() ) );

				return ValidateResult::Invalid{ { lease.toString() } };
			}

			if ( restriction.maxInclusive() && number > *restriction.maxInclusive() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Value must be less than or equal to " );
				builder.append( std::to_string( *restriction.maxInclusive() ) );

				return ValidateResult::Invalid{ { lease.toString() } };
			}

			return ValidateResult::Ok{};
		}
	}
}
