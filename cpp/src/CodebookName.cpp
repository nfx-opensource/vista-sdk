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

	std::string CodebookNames::toPrefix( CodebookName name )
	{
		std::string_view result;

		switch ( name )
		{
			case CodebookName::Position:
			{
				result = internal::constants::codebook::PREFIX_POSITION;
				break;
			}
			case CodebookName::Quantity:
			{
				result = internal::constants::codebook::PREFIX_QUANTITY;
				break;
			}
			case CodebookName::Calculation:
			{
				result = internal::constants::codebook::PREFIX_CALCULATION;
				break;
			}
			case CodebookName::State:
			{
				result = internal::constants::codebook::PREFIX_STATE;
				break;
			}
			case CodebookName::Content:
			{
				result = internal::constants::codebook::PREFIX_CONTENT;
				break;
			}
			case CodebookName::Command:
			{
				result = internal::constants::codebook::PREFIX_COMMAND;
				break;
			}
			case CodebookName::Type:
			{
				result = internal::constants::codebook::PREFIX_TYPE;
				break;
			}
			case CodebookName::FunctionalServices:
			{
				result = internal::constants::codebook::PREFIX_FUNCTIONAL_SERVICES;
				break;
			}
			case CodebookName::MaintenanceCategory:
			{
				result = internal::constants::codebook::PREFIX_MAINTENANCE_CATEGORY;
				break;
			}
			case CodebookName::ActivityType:
			{
				result = internal::constants::codebook::PREFIX_ACTIVITY_TYPE;
				break;
			}
			case CodebookName::Detail:
			{
				result = internal::constants::codebook::PREFIX_DETAIL;
				break;
			}
			default:
			{
				throw std::invalid_argument{ "Unknown codebook: " + std::to_string( static_cast<int>( name ) ) };
				break;
			}
		}

		return std::string{ result };
	}

	CodebookName CodebookNames::fromString( std::string_view name )
	{
		switch ( name.size() )
		{
			case 5:
			{
				if ( name == internal::constants::codebook::NAME_TYPE )
				{
					return CodebookName::Type;
				}
				break;
			}
			case 6:
			{
				if ( name == internal::constants::codebook::NAME_STATE )
				{
					return CodebookName::State;
				}
				if ( name == internal::constants::codebook::NAME_DETAIL )
				{
					return CodebookName::Detail;
				}
				break;
			}
			case 8:
			{
				if ( name == internal::constants::codebook::NAME_CONTENT )
				{
					return CodebookName::Content;
				}
				if ( name == internal::constants::codebook::NAME_COMMAND )
				{
					return CodebookName::Command;
				}
				break;
			}
			case 9:
			{
				if ( name == internal::constants::codebook::NAME_POSITION )
				{
					return CodebookName::Position;
				}
				break;
			}
			case 10:
			{
				if ( name == internal::constants::codebook::NAME_QUANTITY )
				{
					return CodebookName::Quantity;
				}
				break;
			}
			case 12:
			{
				if ( name == internal::constants::codebook::NAME_CALCULATION )
				{
					return CodebookName::Calculation;
				}
				break;
			}
			case 13:
			{
				if ( name == internal::constants::codebook::NAME_ACTIVITY_TYPE )
				{
					return CodebookName::ActivityType;
				}
				break;
			}
			case 19:
			{
				if ( name == internal::constants::codebook::NAME_FUNCTIONAL_SERVICES )
				{
					return CodebookName::FunctionalServices;
				}
				break;
			}
			case 20:
			{
				if ( name == internal::constants::codebook::NAME_MAINTENANCE_CATEGORY )
				{
					return CodebookName::MaintenanceCategory;
				}
				break;
			}
			default:
			{
				break;
			}
		}
		throw std::invalid_argument{ "Unknown codebook name: " + std::string{ name } };
	}

	std::string CodebookNames::toString( CodebookName name )
	{
		std::string_view result;
		switch ( name )
		{
			case CodebookName::Position:
			{
				result = internal::constants::codebook::NAME_POSITION;
				break;
			}
			case CodebookName::Quantity:
			{
				result = internal::constants::codebook::ENUM_QUANTITY;
				break;
			}
			case CodebookName::Calculation:
			{
				result = internal::constants::codebook::ENUM_CALCULATION;
				break;
			}
			case CodebookName::State:
			{
				result = internal::constants::codebook::ENUM_STATE;
				break;
			}
			case CodebookName::Content:
			{
				result = internal::constants::codebook::ENUM_CONTENT;
				break;
			}
			case CodebookName::Command:
			{
				result = internal::constants::codebook::ENUM_COMMAND;
				break;
			}
			case CodebookName::Type:
			{
				result = internal::constants::codebook::ENUM_TYPE;
				break;
			}
			case CodebookName::FunctionalServices:
			{
				result = internal::constants::codebook::ENUM_FUNCTIONAL_SERVICES;
				break;
			}
			case CodebookName::MaintenanceCategory:
			{
				result = internal::constants::codebook::ENUM_MAINTENANCE_CATEGORY;
				break;
			}
			case CodebookName::ActivityType:
			{
				result = internal::constants::codebook::ENUM_ACTIVITY_TYPE;
				break;
			}
			case CodebookName::Detail:
			{
				result = internal::constants::codebook::ENUM_DETAIL;
				break;
			}
			default:
			{
				throw std::invalid_argument{ "Unknown codebook: " + std::to_string( static_cast<int>( name ) ) };
				break;
			}
		}

		return std::string{ result };
	}
}
