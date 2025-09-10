/**
 * @file CodebookName.cpp
 * @brief Implementation of CodebookName utilities
 */

#include <stdexcept>
#include <string>

#include "internal/constants/Codebook.h"

#include "dnv/vista/sdk/CodebookName.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// CodebookNames class
	//=====================================================================

	//----------------------------------------------
	// Public static methods
	//----------------------------------------------

	CodebookName CodebookNames::fromPrefix( std::string_view prefix )
	{
		if ( prefix.empty() )
		{
			throw std::invalid_argument{ "Prefix cannot be empty." };
		}
		if ( prefix == internal::constants::codebook::PREFIX_POSITION )
		{
			return CodebookName::Position;
		}
		if ( prefix == internal::constants::codebook::PREFIX_QUANTITY )
		{
			return CodebookName::Quantity;
		}
		if ( prefix == internal::constants::codebook::PREFIX_STATE )
		{
			return CodebookName::State;
		}
		if ( prefix == internal::constants::codebook::PREFIX_CONTENT )
		{
			return CodebookName::Content;
		}
		if ( prefix == internal::constants::codebook::PREFIX_COMMAND )
		{
			return CodebookName::Command;
		}
		if ( prefix == internal::constants::codebook::PREFIX_TYPE )
		{
			return CodebookName::Type;
		}
		if ( prefix == internal::constants::codebook::PREFIX_CALCULATION )
		{
			return CodebookName::Calculation;
		}
		if ( prefix == internal::constants::codebook::PREFIX_DETAIL )
		{
			return CodebookName::Detail;
		}
		if ( prefix == internal::constants::codebook::PREFIX_FUNCTIONAL_SERVICES )
		{
			return CodebookName::FunctionalServices;
		}
		if ( prefix == internal::constants::codebook::PREFIX_MAINTENANCE_CATEGORY )
		{
			return CodebookName::MaintenanceCategory;
		}
		if ( prefix == internal::constants::codebook::PREFIX_ACTIVITY_TYPE )
		{
			return CodebookName::ActivityType;
		}

		throw std::invalid_argument{ "Unknown prefix: " + std::string{ prefix } };
	}

	std::string_view CodebookNames::toPrefix( CodebookName name )
	{
		switch ( name )
		{
			case CodebookName::Position:
			{
				return internal::constants::codebook::PREFIX_POSITION;
			}
			case CodebookName::Quantity:
			{
				return internal::constants::codebook::PREFIX_QUANTITY;
			}
			case CodebookName::Calculation:
			{
				return internal::constants::codebook::PREFIX_CALCULATION;
			}
			case CodebookName::State:
			{
				return internal::constants::codebook::PREFIX_STATE;
			}
			case CodebookName::Content:
			{
				return internal::constants::codebook::PREFIX_CONTENT;
			}
			case CodebookName::Command:
			{
				return internal::constants::codebook::PREFIX_COMMAND;
			}
			case CodebookName::Type:
			{
				return internal::constants::codebook::PREFIX_TYPE;
			}
			case CodebookName::FunctionalServices:
			{
				return internal::constants::codebook::PREFIX_FUNCTIONAL_SERVICES;
			}
			case CodebookName::MaintenanceCategory:
			{
				return internal::constants::codebook::PREFIX_MAINTENANCE_CATEGORY;
			}
			case CodebookName::ActivityType:
			{
				return internal::constants::codebook::PREFIX_ACTIVITY_TYPE;
			}
			case CodebookName::Detail:
			{
				return internal::constants::codebook::PREFIX_DETAIL;
			}
			default:
			{
				throw std::invalid_argument{ "Unknown codebook: " + std::to_string( static_cast<int>( name ) ) };
			}
		}
	}
}
