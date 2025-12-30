/**
 * @file Results.h
 * @brief Common result types for validation operations
 */

#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace dnv::vista::sdk::transport
{
    /**
     * @brief Result of validation operations
     * @details Contains validation status and error messages if validation failed.
     *          Template parameter T is optional value type for parsing operations.
     * @tparam T Optional value type (void for simple validation, concrete type for parsing)
     */
    template <typename T = void>
    struct ValidateResult final
    {
        bool isValid;                    ///< Validation status flag
        std::vector<std::string> errors; ///< Error messages if validation failed
        [[no_unique_address]] std::conditional_t<std::is_void_v<T>, std::monostate, std::optional<T>>
            value; ///< Parsed value if validation succeeded

        /**
         * @brief Check if validation succeeded
         * @return true if the value is valid
         */
        [[nodiscard]] bool isOk() const noexcept
        {
            return isValid;
        }

        /**
         * @brief Check if validation failed
         * @return true if the value is invalid
         */
        [[nodiscard]] bool isInvalid() const noexcept
        {
            return !isValid;
        }

        /**
         * @brief Create a successful validation result (simple validation)
         * @return ValidateResult indicating success
         */
        [[nodiscard]] static ValidateResult ok()
            requires std::is_void_v<T>
        {
            return ValidateResult{ true, {}, std::monostate{} };
        }

        /**
         * @brief Create a successful validation result with parsed value
         * @param val Parsed value
         * @return ValidateResult indicating success with value
         */
        template <typename U = T>
        [[nodiscard]] static ValidateResult ok( U&& val )
            requires( !std::is_void_v<T> )
        {
            return ValidateResult{ true, {}, std::optional<T>{ std::forward<U>( val ) } };
        }

        /**
         * @brief Create a failed validation result
         * @param errors Vector of error messages describing validation failures
         * @return ValidateResult indicating failure
         */
        [[nodiscard]] static ValidateResult invalid( std::vector<std::string> errors )
        {
            if constexpr( std::is_void_v<T> )
            {
                return ValidateResult{ false, std::move( errors ), std::monostate{} };
            }
            else
            {
                return ValidateResult{ false, std::move( errors ), std::nullopt };
            }
        }
    };

    /**
     * @brief Result of parsing operations
     * @details Contains parse status and optional value if parsing succeeded.
     * @tparam T Type of parsed value
     */
    template <typename T>
    struct ParseResult final
    {
        bool isValid;           ///< Parse status flag
        std::optional<T> value; ///< Parsed value if parsing succeeded

        /**
         * @brief Check if parsing succeeded
         * @return true if a valid value was parsed
         */
        [[nodiscard]] bool isOk() const noexcept
        {
            return isValid;
        }

        /**
         * @brief Check if parsing failed
         * @return true if parsing failed
         */
        [[nodiscard]] bool isInvalid() const noexcept
        {
            return !isValid;
        }

        /**
         * @brief Create a successful parse result
         * @param val The parsed value
         * @return ParseResult indicating success
         */
        template <typename U = T>
        [[nodiscard]] static ParseResult ok( U&& val )
        {
            return ParseResult{ true, std::optional<T>{ std::forward<U>( val ) } };
        }

        /**
         * @brief Create a failed parse result
         * @return ParseResult indicating failure
         */
        [[nodiscard]] static ParseResult invalid()
        {
            return ParseResult{ false, std::nullopt };
        }
    };
} // namespace dnv::vista::sdk::transport
