/**
 * @file ISO19848.h
 * @brief ISO 19848 Time Series Data specifications
 * @details Provides types and utilities for working with ISO 19848 time series data,
 *          including data channel type names, format data types, and value representations.
 */

#pragma once

#include "ISO19848Versions.h"
#include "DateTime.h"
#include "Decimal.h"
#include "Results.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace dnv::vista::sdk::transport
{
    /**
     * @brief Represents a data channel type name entry from the ISO 19848 specification
     */
    struct DataChannelTypeName final
    {
        std::string type;        ///< The type identifier
        std::string description; ///< Human-readable description

        /** @brief Default constructor */
        DataChannelTypeName() = delete;

        /** @brief Copy constructor */
        DataChannelTypeName( const DataChannelTypeName& ) = default;

        /** @brief Move constructor */
        DataChannelTypeName( DataChannelTypeName&& ) noexcept = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this DataChannelTypeName
         */
        DataChannelTypeName& operator=( const DataChannelTypeName& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this DataChannelTypeName
         */
        DataChannelTypeName& operator=( DataChannelTypeName&& ) noexcept = default;

        /**
         * @brief Construct a data channel type name
         * @param type The type identifier
         * @param description Human-readable description
         */
        inline explicit DataChannelTypeName( std::string type, std::string description );

        /**
         * @brief Equality comparison operator
         * @param other The DataChannelTypeName to compare with
         * @return true if both type and description are equal
         */
        [[nodiscard]] bool operator==( const DataChannelTypeName& other ) const noexcept = default;
    };

    /**
     * @brief Collection of data channel type names from ISO 19848
     */
    class DataChannelTypeNames final
    {
    public:
        /** @brief Const iterator type for iterating over data channel type names */
        using ConstIterator = std::vector<DataChannelTypeName>::const_iterator;

        /** @brief Default constructor */
        DataChannelTypeNames() = delete;

        /**
         * @brief Construct from a vector of data channel type names
         * @param values Vector of DataChannelTypeName entries
         */
        inline explicit DataChannelTypeNames( std::vector<DataChannelTypeName> values );

        /** @brief Copy constructor */
        DataChannelTypeNames( const DataChannelTypeNames& ) = default;

        /** @brief Move constructor */
        DataChannelTypeNames( DataChannelTypeNames&& ) noexcept = default;

        /** @brief Destructor */
        ~DataChannelTypeNames() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this DataChannelTypeNames
         */
        DataChannelTypeNames& operator=( const DataChannelTypeNames& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this DataChannelTypeNames
         */
        DataChannelTypeNames& operator=( DataChannelTypeNames&& ) noexcept = default;

        /**
         * @brief Parse a type string to find matching DataChannelTypeName
         * @param dataChannelTypeName The type identifier to look up
         * @return ParseResult containing the type name if found
         */
        [[nodiscard]] ParseResult<DataChannelTypeName> fromString( std::string_view dataChannelTypeName ) const;

        /**
         * @brief Get iterator to first element
         * @return Iterator to the first data channel type name
         */
        [[nodiscard]] inline ConstIterator begin() const noexcept;

        /**
         * @brief Get iterator past last element
         * @return Iterator to the element following the last data channel type name
         */
        [[nodiscard]] inline ConstIterator end() const noexcept;

        /**
         * @brief Get number of elements
         * @return Number of data channel type names in the collection
         */
        [[nodiscard]] inline size_t size() const noexcept;

        /**
         * @brief Check if collection is empty
         * @return true if the collection contains no elements
         */
        [[nodiscard]] inline bool isEmpty() const noexcept;

    private:
        std::vector<DataChannelTypeName> m_values;
    };

    /**
     * @brief Represents a value in ISO 19848 format
     * @details A discriminated union that can hold one of several primitive types
     *          used in ISO 19848 time series data.
     */
    class Value final
    {
    public:
        /** @brief Enumeration representing the variant types */
        enum class Type : std::uint8_t
        {
            Decimal = 0,
            Integer,
            Boolean,
            String,
            DateTime
        };

        /** @brief Explicit String wrapper */
        struct String
        {
            std::string value; ///< The string value
        };

        /** @brief Explicit Integer wrapper */
        struct Integer
        {
            int32_t value; ///< The integer value
        };

        /** @brief Explicit Boolean wrapper */
        struct Boolean
        {
            bool value; ///< The boolean value
        };

        /** @brief Explicit Decimal wrapper */
        struct Decimal
        {
            sdk::Decimal value; ///< The decimal value

            /**
             * @brief Construct from string
             * @param str String representation of decimal number
             */
            inline explicit Decimal( std::string_view str );

            /**
             * @brief Construct from float
             * @param f Float value
             */
            inline explicit Decimal( float f );

            /**
             * @brief Construct from double
             * @param d Double value
             */
            inline explicit Decimal( double d );

            /**
             * @brief Construct from sdk::Decimal
             * @param dec Decimal value
             */
            inline explicit Decimal( sdk::Decimal dec );
        };

        /** @brief Explicit DateTime wrapper */
        struct DateTime
        {
            sdk::DateTime value; ///< The datetime value

            /**
             * @brief Construct from string (ISO 8601 format)
             * @param str ISO 8601 formatted datetime string
             */
            inline explicit DateTime( std::string_view str );

            /**
             * @brief Construct from sdk::DateTime
             * @param dt DateTime value
             */
            inline explicit DateTime( sdk::DateTime dt );

            /**
             * @brief Construct from sdk::DateTimeOffset
             * @param dto DateTimeOffset value
             */
            inline explicit DateTime( sdk::DateTimeOffset dto );
        };

        /** @brief Default constructor */
        Value() = delete;

        /** @brief Copy constructor */
        Value( const Value& ) = default;

        /** @brief Move constructor */
        Value( Value&& ) noexcept = default;

        /** @brief Destructor */
        ~Value() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this Value
         */
        Value& operator=( const Value& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this Value
         */
        Value& operator=( Value&& ) noexcept = default;

        /**
         * @brief Equality comparison operator
         * @param other The Value to compare with
         * @return true if both values are equal
         */
        [[nodiscard]] bool operator==( const Value& other ) const noexcept = default;

        /**
         * @brief Construct from String wrapper
         * @param s String wrapper
         */
        inline Value( String s );

        /**
         * @brief Construct from Integer wrapper
         * @param i Integer wrapper
         */
        inline Value( Integer i );

        /**
         * @brief Construct from Boolean wrapper
         * @param b Boolean wrapper
         */
        inline Value( Boolean b );

        /**
         * @brief Construct from Decimal wrapper
         * @param d Decimal wrapper
         */
        inline Value( Decimal d );

        /**
         * @brief Construct from DateTime wrapper
         * @param dt DateTime wrapper
         */
        inline Value( DateTime dt );

        // Primitive type constructors are deleted to enforce explicit wrapper usage
        Value( const char* ) = delete;
        Value( std::string ) = delete;
        Value( std::string_view ) = delete;
        Value( bool ) = delete;
        Value( int32_t ) = delete;
        Value( int64_t ) = delete;
        Value( uint32_t ) = delete;
        Value( uint64_t ) = delete;
        Value( int16_t ) = delete;
        Value( uint16_t ) = delete;
        Value( int8_t ) = delete;
        Value( uint8_t ) = delete;
        Value( float ) = delete;
        Value( double ) = delete;

        /**
         * @brief Get String value if present
         * @return String value or std::nullopt if value is not a String
         */
        [[nodiscard]] inline std::optional<std::string_view> string() const noexcept;

        /**
         * @brief Get Boolean value if present
         * @return Boolean value or std::nullopt if value is not a Boolean
         */
        [[nodiscard]] inline std::optional<bool> boolean() const noexcept;

        /**
         * @brief Get Integer value if present
         * @return Integer value or std::nullopt if value is not an Integer
         */
        [[nodiscard]] inline std::optional<int32_t> integer() const noexcept;

        /**
         * @brief Get Decimal value if present
         * @return Decimal value or std::nullopt if value is not a Decimal
         */
        [[nodiscard]] inline std::optional<sdk::Decimal> decimal() const noexcept;

        /**
         * @brief Get DateTime value if present
         * @return DateTime value or std::nullopt if value is not a DateTime
         */
        [[nodiscard]] inline std::optional<sdk::DateTime> dateTime() const noexcept;

        /**
         * @brief Convert the value to its string representation
         * @return String representation of the contained value
         */
        [[nodiscard]] inline std::string toString() const;

        /**
         * @brief Get the type as an enum for readable switch statements
         * @return Type enum representing the currently held type
         *@note This function is marked [[nodiscard]] - the return value should not be ignored
         */
        [[nodiscard]] inline Type type() const noexcept;

        /**
         * @brief Pattern matching over the value
         * @tparam FDecimal Callable type for Decimal values
         * @tparam FInteger Callable type for Integer values
         * @tparam FBoolean Callable type for Boolean values
         * @tparam FString Callable type for String values
         * @tparam FDateTime Callable type for DateTime values
         * @param onDecimal Handler for Decimal values
         * @param onInteger Handler for Integer values
         * @param onBoolean Handler for Boolean values
         * @param onString Handler for String values
         * @param onDateTime Handler for DateTime values
         * @return Result of calling the appropriate handler based on the contained type
         */
        template <typename FDecimal, typename FInteger, typename FBoolean, typename FString, typename FDateTime>
        [[nodiscard]] inline auto match(
            FDecimal&& onDecimal,
            FInteger&& onInteger,
            FBoolean&& onBoolean,
            FString&& onString,
            FDateTime&& onDateTime ) const;

        /**
         * @brief Visit pattern for side effects
         * @tparam FDecimal Callable type for Decimal values
         * @tparam FInteger Callable type for Integer values
         * @tparam FBoolean Callable type for Boolean values
         * @tparam FString Callable type for String values
         * @tparam FDateTime Callable type for DateTime values
         * @param onDecimal Handler for Decimal values
         * @param onInteger Handler for Integer values
         * @param onBoolean Handler for Boolean values
         * @param onString Handler for String values
         * @param onDateTime Handler for DateTime values
         */
        template <typename FDecimal, typename FInteger, typename FBoolean, typename FString, typename FDateTime>
        void visit(
            FDecimal&& onDecimal,
            FInteger&& onInteger,
            FBoolean&& onBoolean,
            FString&& onString,
            FDateTime&& onDateTime ) const;

    private:
        std::variant<sdk::Decimal, int32_t, bool, std::string, sdk::DateTime> m_value;
    };

    /**
     * @brief Stream insertion operator for Value
     * @param os Output stream
     * @param value The Value to output
     * @return Reference to the output stream
     */
    inline std::ostream& operator<<( std::ostream& os, const Value& value )
    {
        return os << value.toString();
    }

    /**
     * @brief Represents a format data type entry from the ISO 19848 specification
     */
    struct FormatDataType final
    {
        std::string type;        ///< The type identifier
        std::string description; ///< Human-readable description

        /** @brief Default constructor */
        FormatDataType() = delete;

        /** @brief Copy constructor */
        FormatDataType( const FormatDataType& ) = default;

        /** @brief Move constructor */
        FormatDataType( FormatDataType&& ) noexcept = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this FormatDataType
         */
        FormatDataType& operator=( const FormatDataType& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this FormatDataType
         */
        FormatDataType& operator=( FormatDataType&& ) noexcept = default;

        /**
         * @brief Construct a format data type
         * @param type The type identifier
         * @param description Human-readable description
         */
        inline explicit FormatDataType( std::string type, std::string description );

        /**
         * @brief Equality comparison operator
         * @param other The FormatDataType to compare with
         * @return true if both type and description are equal
         */
        [[nodiscard]] bool operator==( const FormatDataType& other ) const noexcept = default;

        /**
         * @brief Validate a string value against this format type
         * @param value The string value to validate
         * @return Validation result with parsed value if successful
         */
        [[nodiscard]] ValidateResult<Value> validate( std::string_view value ) const;

        /**
         * @brief Pattern matching over validated value with type checking
         * @tparam FDecimal Callable type for Decimal values
         * @tparam FInteger Callable type for Integer values
         * @tparam FBoolean Callable type for Boolean values
         * @tparam FString Callable type for String values
         * @tparam FDateTime Callable type for DateTime values
         * @param value The string value to validate and match
         * @param onDecimal Handler for Decimal values
         * @param onInteger Handler for Integer values
         * @param onBoolean Handler for Boolean values
         * @param onString Handler for String values
         * @param onDateTime Handler for DateTime values
         * @return Result of calling the appropriate handler based on the validated type
         * @throws std::invalid_argument if validation fails
         */
        template <typename FDecimal, typename FInteger, typename FBoolean, typename FString, typename FDateTime>
        [[nodiscard]] inline auto match(
            std::string_view value,
            FDecimal&& onDecimal,
            FInteger&& onInteger,
            FBoolean&& onBoolean,
            FString&& onString,
            FDateTime&& onDateTime ) const;
    };

    /**
     * @brief Collection of format data types from ISO 19848
     */
    class FormatDataTypes final
    {
    public:
        /** @brief Const iterator type for iterating over format data types */
        using ConstIterator = std::vector<FormatDataType>::const_iterator;

        FormatDataTypes() = delete;

        /**
         * @brief Construct from a vector of format data types
         * @param values Vector of FormatDataType entries
         */
        inline explicit FormatDataTypes( std::vector<FormatDataType> values );

        /** @brief Copy constructor */
        FormatDataTypes( const FormatDataTypes& ) = default;

        /** @brief Move constructor */
        FormatDataTypes( FormatDataTypes&& ) noexcept = default;

        /** @brief Destructor */
        ~FormatDataTypes() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this FormatDataTypes
         */
        FormatDataTypes& operator=( const FormatDataTypes& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this FormatDataTypes
         */
        FormatDataTypes& operator=( FormatDataTypes&& ) noexcept = default;

        /**
         * @brief Parse a type string to find matching FormatDataType
         * @param formatDataType The type identifier to look up
         * @return ParseResult containing the type name if found
         */
        [[nodiscard]] ParseResult<FormatDataType> fromString( std::string_view formatDataType ) const;

        /**
         * @brief Get iterator to first element
         * @return Iterator to the first format data type
         */
        [[nodiscard]] inline ConstIterator begin() const noexcept;

        /**
         * @brief Get iterator past last element
         * @return Iterator to the element following the last format data type
         */
        [[nodiscard]] inline ConstIterator end() const noexcept;

        /**
         * @brief Get number of elements
         * @return Number of format data types in the collection
         */
        [[nodiscard]] inline size_t size() const noexcept;

        /**
         * @brief Check if collection is empty
         * @return true if the collection contains no elements
         */
        [[nodiscard]] inline bool isEmpty() const noexcept;

    private:
        std::vector<FormatDataType> m_values;
    };

    /**
     * @brief Main ISO 19848 interface for accessing type definitions
     */
    class ISO19848 final
    {
    private:
        /** @brief Default constructor */
        ISO19848() = default;

        /** @brief Copy constructor */
        ISO19848( const ISO19848& ) = delete;

        /** @brief Move constructor */
        ISO19848( ISO19848&& ) noexcept = delete;

        /** @brief Destructor */
        ~ISO19848() = default;

        /** @brief Copy assignment operator */
        ISO19848& operator=( const ISO19848& ) = delete;

        /** @brief Move assignment operator */
        ISO19848& operator=( ISO19848&& ) noexcept = delete;

    public:
        /**
         * @brief Get the singleton instance
         * @return Reference to the global ISO19848 instance
         */
        [[nodiscard]] static ISO19848& instance();

        /**
         * @brief Get all available ISO 19848 versions
         * @return Vector containing all supported ISO 19848 versions in lexicographical order
         */
        [[nodiscard]] const std::vector<ISO19848Version>& versions() const noexcept;

        /**
         * @brief Get the latest ISO 19848 version
         * @return The most recent ISO 19848 version
         */
        [[nodiscard]] ISO19848Version latest() const noexcept;

        /**
         * @brief Get data channel type names for a specific version
         * @param version The ISO 19848 version
         * @return DataChannelTypeNames collection for the specified version
         */
        [[nodiscard]] DataChannelTypeNames dataChannelTypeNames( ISO19848Version version );

        /**
         * @brief Get format data types for a specific version
         * @param version The ISO 19848 version
         * @return FormatDataTypes collection
         */
        [[nodiscard]] FormatDataTypes formatDataTypes( ISO19848Version version );
    };
} // namespace dnv::vista::sdk::transport

#include "dnv/vista/sdk/detail/transport/ISO19848.inl"
