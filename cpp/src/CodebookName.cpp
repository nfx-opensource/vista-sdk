/**
 * @file CodebookName.cpp
 * @brief Implementation of CodebookName utilities
 */

#include <stdexcept>

#include <nfx/string/Utils.h>

#include "internal/core/Codebook.h"
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
		if ( nfx::string::isEmpty( prefix ) )
		{
			throw std::invalid_argument{ "Prefix cannot be empty." };
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_POSITION ) )
		{
			return CodebookName::Position;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_QUANTITY ) )
		{
			return CodebookName::Quantity;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_STATE ) )
		{
			return CodebookName::State;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_CONTENT ) )
		{
			return CodebookName::Content;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_COMMAND ) )
		{
			return CodebookName::Command;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_TYPE ) )
		{
			return CodebookName::Type;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_CALCULATION ) )
		{
			return CodebookName::Calculation;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_DETAIL ) )
		{
			return CodebookName::Detail;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_FUNCTIONAL_SERVICES ) )
		{
			return CodebookName::FunctionalServices;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_MAINTENANCE_CATEGORY ) )
		{
			return CodebookName::MaintenanceCategory;
		}
		if ( nfx::string::equals( prefix, internal::codebook::PREFIX_ACTIVITY_TYPE ) )
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
				result = internal::codebook::PREFIX_POSITION;
				break;
			}
			case CodebookName::Quantity:
			{
				result = internal::codebook::PREFIX_QUANTITY;
				break;
			}
			case CodebookName::Calculation:
			{
				result = internal::codebook::PREFIX_CALCULATION;
				break;
			}
			case CodebookName::State:
			{
				result = internal::codebook::PREFIX_STATE;
				break;
			}
			case CodebookName::Content:
			{
				result = internal::codebook::PREFIX_CONTENT;
				break;
			}
			case CodebookName::Command:
			{
				result = internal::codebook::PREFIX_COMMAND;
				break;
			}
			case CodebookName::Type:
			{
				result = internal::codebook::PREFIX_TYPE;
				break;
			}
			case CodebookName::FunctionalServices:
			{
				result = internal::codebook::PREFIX_FUNCTIONAL_SERVICES;
				break;
			}
			case CodebookName::MaintenanceCategory:
			{
				result = internal::codebook::PREFIX_MAINTENANCE_CATEGORY;
				break;
			}
			case CodebookName::ActivityType:
			{
				result = internal::codebook::PREFIX_ACTIVITY_TYPE;
				break;
			}
			case CodebookName::Detail:
			{
				result = internal::codebook::PREFIX_DETAIL;
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
				if ( nfx::string::equals( name, internal::codebook::NAME_TYPE ) )
				{
					return CodebookName::Type;
				}
				break;
			}
			case 6:
			{
				if ( nfx::string::equals( name, internal::codebook::NAME_STATE ) )
				{
					return CodebookName::State;
				}
				if ( nfx::string::equals( name, internal::codebook::NAME_DETAIL ) )
				{
					return CodebookName::Detail;
				}
				break;
			}
			case 8:
			{
				if ( nfx::string::equals( name, internal::codebook::NAME_CONTENT ) )
				{
					return CodebookName::Content;
				}
				if ( nfx::string::equals( name, internal::codebook::NAME_COMMAND ) )
				{
					return CodebookName::Command;
				}
				break;
			}
			case 9:
			{
				if ( nfx::string::equals( name, internal::codebook::NAME_POSITION ) )
				{
					return CodebookName::Position;
				}
				break;
			}
			case 10:
			{
				if ( nfx::string::equals( name, internal::codebook::NAME_QUANTITY ) )
				{
					return CodebookName::Quantity;
				}
				break;
			}
			case 12:
			{
				if ( nfx::string::equals( name, internal::codebook::NAME_CALCULATION ) )
				{
					return CodebookName::Calculation;
				}
				break;
			}
			case 13:
			{
				if ( nfx::string::equals( name, internal::codebook::NAME_ACTIVITY_TYPE ) )
				{
					return CodebookName::ActivityType;
				}
				break;
			}
			case 19:
			{
				if ( nfx::string::equals( name, internal::codebook::NAME_FUNCTIONAL_SERVICES ) )
				{
					return CodebookName::FunctionalServices;
				}
				break;
			}
			case 20:
			{
				if ( nfx::string::equals( name, internal::codebook::NAME_MAINTENANCE_CATEGORY ) )
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
				result = internal::codebook::NAME_POSITION;
				break;
			}
			case CodebookName::Quantity:
			{
				result = internal::codebook::ENUM_QUANTITY;
				break;
			}
			case CodebookName::Calculation:
			{
				result = internal::codebook::ENUM_CALCULATION;
				break;
			}
			case CodebookName::State:
			{
				result = internal::codebook::ENUM_STATE;
				break;
			}
			case CodebookName::Content:
			{
				result = internal::codebook::ENUM_CONTENT;
				break;
			}
			case CodebookName::Command:
			{
				result = internal::codebook::ENUM_COMMAND;
				break;
			}
			case CodebookName::Type:
			{
				result = internal::codebook::ENUM_TYPE;
				break;
			}
			case CodebookName::FunctionalServices:
			{
				result = internal::codebook::ENUM_FUNCTIONAL_SERVICES;
				break;
			}
			case CodebookName::MaintenanceCategory:
			{
				result = internal::codebook::ENUM_MAINTENANCE_CATEGORY;
				break;
			}
			case CodebookName::ActivityType:
			{
				result = internal::codebook::ENUM_ACTIVITY_TYPE;
				break;
			}
			case CodebookName::Detail:
			{
				result = internal::codebook::ENUM_DETAIL;
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
