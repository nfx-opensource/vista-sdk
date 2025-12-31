/**
 * @file ISO19848Validation.h
 * @brief ISO 19848 format validation utilities
 * @details Provides validation functions for ISO 19848 data formats
 */

#pragma once

#include <regex>
#include <string_view>

namespace nfx::serialization::json
{
    /**
     * @brief Validates DateTimeOffset string against ISO 19848 strict format
     * @note Stricter than ISO 8601 - requires ±hh:mm (not ±hh)
     * @param str The string to validate
     * @return true if the string matches the ISO 19848 DateTimeOffset format, false otherwise
     */
    inline bool isValidIso19848DateTimeOffset( std::string_view str )
    {
        static const std::regex pattern{ R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[+-]\d{2}:\d{2})$)" };
        return std::regex_match( str.begin(), str.end(), pattern );
    }
} // namespace nfx::serialization::json
