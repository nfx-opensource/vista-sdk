/**
 * @file CodebookName.inl
 * @brief CodebookName inline implementations
 * @details Inline method implementations for CodebookName conversions
 */

#include <stdexcept>

namespace dnv::vista::sdk::CodebookNames
{
    inline CodebookName fromPrefix( std::string_view prefix )
    {
        if( prefix == "qty" )
        {
            return CodebookName::Quantity;
        }
        if( prefix == "cnt" )
        {
            return CodebookName::Content;
        }
        if( prefix == "calc" )
        {
            return CodebookName::Calculation;
        }
        if( prefix == "state" )
        {
            return CodebookName::State;
        }
        if( prefix == "cmd" )
        {
            return CodebookName::Command;
        }
        if( prefix == "type" )
        {
            return CodebookName::Type;
        }
        if( prefix == "funct.svc" )
        {
            return CodebookName::FunctionalServices;
        }
        if( prefix == "maint.cat" )
        {
            return CodebookName::MaintenanceCategory;
        }
        if( prefix == "act.type" )
        {
            return CodebookName::ActivityType;
        }
        if( prefix == "pos" )
        {
            return CodebookName::Position;
        }
        if( prefix == "detail" )
        {
            return CodebookName::Detail;
        }

        throw std::invalid_argument{ "Unknown prefix: " + std::string{ prefix } };
    }

    inline std::string toPrefix( CodebookName name )
    {
        switch( name )
        {
            case CodebookName::Quantity:
            {
                return "qty";
            }
            case CodebookName::Content:
            {
                return "cnt";
            }
            case CodebookName::Calculation:
            {
                return "calc";
            }
            case CodebookName::State:
            {
                return "state";
            }
            case CodebookName::Command:
            {
                return "cmd";
            }
            case CodebookName::Type:
            {
                return "type";
            }
            case CodebookName::FunctionalServices:
            {
                return "funct.svc";
            }
            case CodebookName::MaintenanceCategory:
            {
                return "maint.cat";
            }
            case CodebookName::ActivityType:
            {
                return "act.type";
            }
            case CodebookName::Position:
            {
                return "pos";
            }
            case CodebookName::Detail:
            {
                return "detail";
            }
            default:
            {
                throw std::invalid_argument{ "Unknown codebook: " +
                                             std::to_string( static_cast<std::uint8_t>( name ) ) };
                break;
            }
        }

        return "";
    }
} // namespace dnv::vista::sdk::CodebookNames
