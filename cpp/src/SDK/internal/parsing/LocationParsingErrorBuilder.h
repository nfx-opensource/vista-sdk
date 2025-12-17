/**
 * @file LocationParsingErrorBuilder.h
 * @brief Error builder for Location parsing validation
 * @details Accumulates validation errors during Location string parsing with specific error types
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
    class ParsingErrors;

    namespace internal
    {
        /**
         * @enum LocationValidationResult
         * @brief Result codes for Location string validation
         * @details Indicates the type of validation error encountered during Location parsing
         */
        enum class LocationValidationResult : std::uint8_t
        {
            Invalid = 0,      ///< Invalid format or structure
            InvalidCode,      ///< Invalid location code character
            InvalidOrder,     ///< Location codes not alphabetically sorted
            NullOrWhiteSpace, ///< Empty or whitespace-only string
            Valid             ///< Valid location string
        };

        /**
         * @class LocationParsingErrorBuilder
         * @brief Builder for accumulating Location parsing errors
         * @details Collects validation errors with specific error types during Location string parsing.
         *          Use default constructor to instantiate, then addError() to accumulate errors,
         *          and build() to produce final ParsingErrors object.
         */
        class LocationParsingErrorBuilder final
        {
        public:
            /** @brief Default constructor */
            LocationParsingErrorBuilder() = default;

            /** @brief Copy constructor */
            LocationParsingErrorBuilder( const LocationParsingErrorBuilder& ) = default;

            /** @brief Move constructor */
            LocationParsingErrorBuilder( LocationParsingErrorBuilder&& ) noexcept = default;

            /** @brief Destructor */
            ~LocationParsingErrorBuilder() = default;

            /** @brief Copy assignment operator */
            LocationParsingErrorBuilder& operator=( const LocationParsingErrorBuilder& ) = default;

            /** @brief Move assignment operator */
            LocationParsingErrorBuilder& operator=( LocationParsingErrorBuilder&& ) noexcept = default;

            /**
             * @brief Get empty singleton instance
             * @return Const reference to empty error builder
             */
            [[nodiscard]] static const LocationParsingErrorBuilder& empty() noexcept;

            /**
             * @brief Check if any errors have been accumulated
             * @return True if errors exist, false otherwise
             */
            [[nodiscard]] bool hasError() const noexcept;

            /**
             * @brief Build the final ParsingErrors object
             * @return ParsingErrors containing all accumulated errors
             */
            [[nodiscard]] ParsingErrors build() const;

            /**
             * @brief Add a validation error
             * @param validationResult Type of validation error
             * @param message Error message description
             * @return Reference to this builder for chaining
             */
            LocationParsingErrorBuilder& addError(
                LocationValidationResult validationResult, std::string_view message );

        private:
            std::vector<std::pair<LocationValidationResult, std::string>> m_errors; ///< Accumulated validation errors
        };
    } // namespace internal
} // namespace dnv::vista::sdk
