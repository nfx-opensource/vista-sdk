/**
 * @file ISO19848.h
 * @brief ISO-19848 Standard Implementation for Maritime Data Exchange
 *
 * @details
 * This file implements the complete **ISO-19848 Standard** for maritime data exchange.
 * It provides type-safe validation, standard-compliant data types, and comprehensive
 * format validation for maritime data channel systems.
 *
 * ## Standard Overview:
 *
 * **ISO-19848** defines standardized data channel types and format specifications for:
 * - **Data Channel Classification**: Standard categories for maritime sensor data
 * - **Format Data Types**: Type-safe validation for sensor values and measurements
 * - **Version Management**: Support for multiple standard versions (2018, 2024)
 * - **Compliance Validation**: Ensures data adheres to maritime industry standards
 *
 * ## Core Components:
 *
 * ### Value System (Type-Safe Data Storage)
 * - **Value**              : Discriminated union for all ISO-19848 data types
 * - **Type Safety**        : Compile-time and runtime type validation
 * - **Zero-Copy Design**   : Efficient value storage and retrieval
 * - **Standard Compliance**: Full coverage of ISO format specifications
 *
 * ### Data Channel Types
 * - **DataChannelTypeName** : Individual channel type with validation
 * - **DataChannelTypeNames**: Complete registry of standard channel types
 * - **Lookup Operations**   : Fast O(1) validation and type checking
 * - **Version Support**     : Multiple ISO standard versions
 *
 * ### Format Data Types
 * - **FormatDataType** : Individual format specification with parsing
 * - **FormatDataTypes**: Complete registry of standard format types
 * - **Value Parsing**  : String-to-typed-value conversion with validation
 * - **Error Handling** : Detailed validation error reporting
 *
 * ## Supported Data Types:
 *
 * - **Decimal**        : High-precision decimal numbers with precision/scale validation
 * - **Integer**        : 32-bit signed integers with range validation
 * - **Boolean**        : True/false values with boolean parsing
 * - **String**         : Text values with length/pattern validation
 * - **DateTime**       : ISO 8601 timestamps with format validation
 *
 * ## Usage Examples:
 *
 * ### Data Channel Type Validation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Format Data Type Validation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Type-Safe Value Operations
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Standard Versions:
 *
 * - **ISO-19848:2018**   : Original maritime data exchange standard
 * - **ISO-19848:2024**   : Latest revision with extended data types and validation
 * - **Version Selection**: Configurable version support for legacy compatibility
 * - **Migration Support**: Tools for upgrading between standard versions
 *
 * ## Validation Framework:
 *
 * The implementation provides comprehensive validation at multiple levels:
 * - **Syntax Validation**  : Ensures values match expected format patterns
 * - **Type Validation**    : Verifies values can be parsed to target types
 * - **Range Validation**   : Checks numeric bounds and constraints
 * - **Standard Compliance**: Validates against official ISO specifications
 * - **Error Reporting**    : Detailed error messages for debugging and logging
 *
 * ## Performance Characteristics:
 *
 * - **O(1) Lookups**    : Hash-based type and format lookups
 * - **Zero-Copy Design**: Efficient value storage without unnecessary copying
 * - **Lazy Loading**    : Standard definitions loaded on demand
 * - **Memory Efficient**: Optimized data structures for large-scale usage
 * - **Thread Safe**     : Immutable standard definitions safe for concurrent access
 *
 * ## Design Philosophy:
 *
 * - **Standards Compliance**: Full adherence to ISO-19848 specifications
 * - **Type Safety**         : Compile-time and runtime type safety guarantees
 * - **Performance**         : Optimized for high-throughput maritime data processing
 * - **Extensibility**       : Support for custom extensions while maintaining compliance
 * - **Error Prevention**    : Design patterns that prevent common validation errors
 *
 * @note This implementation is the foundation for all data validation in the Vista SDK.
 *       It ensures that maritime data exchange follows international standards and
 *       provides reliable, type-safe data processing capabilities.
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <nfx/datatypes/Decimal.h>
#include <nfx/DateTime.h>
#include <nfx/datetime/DateTimeOffset.h>

#include "dnv/vista/sdk/Exceptions.h"
#include "dnv/vista/sdk/Results.h"

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// Enumerations
	//=====================================================================

	/** @brief ISO 19848 standard versions */
	enum class ISO19848Version : std::uint8_t
	{
		v2018,
		v2024,

		Latest = v2024
	};

	//=====================================================================
	// Value class
	//=====================================================================

	/**
	 * @brief Typed value wrapper for format data type validation results
	 * @details Supports all ISO 19848 format data types with efficient variant storage
	 */
	class Value
	{
	public:
		//----------------------------------------------
		// Type enumeration
		//----------------------------------------------

		/** @brief Enumeration representing the variant types */
		enum class Type : std::uint8_t
		{
			Decimal = 0,
			Integer,
			Boolean,
			String,
			DateTime
		};

		//----------------------------------------------
		// Value::Decimal class
		//----------------------------------------------

		/**
		 * @brief High-precision decimal value wrapper for format data type validation
		 */
		class Decimal final
		{
		public:
			/**
			 * @brief Construct decimal value from high-precision decimal
			 * @param value The high-precision decimal value to store
			 */
			inline explicit Decimal( const nfx::datatypes::Decimal& value ) noexcept;

			/**
			 * @brief Construct decimal value from double
			 * @param value The double value to convert and store as decimal
			 */
			inline explicit Decimal( double value ) noexcept;

			/**
			 * @brief Get the high-precision decimal value
			 * @return Reference to the internal high-precision decimal value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const nfx::datatypes::Decimal& value() const noexcept;

		private:
			/** @brief Internal high-precision decimal storage */
			nfx::datatypes::Decimal m_value;
		};

		//----------------------------------------------
		// Value::Integer class
		//----------------------------------------------

		/**
		 * @brief Integer value wrapper for format data type validation
		 */
		class Integer final
		{
		public:
			/**
			 * @brief Construct integer value from int (32-bit)
			 * @param value The integer value to store
			 */
			inline explicit Integer( int value ) noexcept;

			/**
			 * @brief Construct integer value from int64_t (64-bit)
			 * @param value The 64-bit integer value to store
			 */
			inline explicit Integer( int64_t value ) noexcept;

			/**
			 * @brief Get the integer value
			 * @return The internal 64-bit integer value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline int64_t value() const noexcept;

		private:
			/** @brief Internal 64-bit integer storage (JSON-compatible) */
			int64_t m_value;
		};

		//----------------------------------------------
		// Value::Boolean class
		//----------------------------------------------

		/**
		 * @brief Boolean value wrapper for format data type validation
		 */
		class Boolean final
		{
		public:
			/**
			 * @brief Construct boolean value from bool
			 * @param value The boolean value to store
			 */
			inline explicit Boolean( bool value ) noexcept;

			/**
			 * @brief Get the boolean value
			 * @return The internal boolean value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool value() const noexcept;

		private:
			/** @brief Internal boolean storage */
			bool m_value;
		};

		//----------------------------------------------
		// Value::String class
		//----------------------------------------------

		/**
		 * @brief String value wrapper for format data type validation
		 */
		class String final
		{
		public:
			/**
			 * @brief Construct string value from string view
			 * @param value The string value to store
			 */
			inline explicit String( std::string_view value ) noexcept;

			/** @brief Default constructor */
			String() = default;

			/** @brief Copy constructor */
			String( const String& ) = default;

			/** @brief Move constructor */
			String( String&& ) noexcept = default;

			/** @brief Destructor */
			~String() = default;

			/**
			 * @brief Copy assignment
			 * @param other The String to copy from
			 * @return Reference to this String
			 */
			String& operator=( const String& other ) = default;

			/**
			 * @brief Move assignment
			 * @param other The String to move from
			 * @return Reference to this String
			 */
			String& operator=( String&& other ) noexcept = default;

			/**
			 * @brief Get the string value
			 * @return Reference to the internal string value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const std::string& value() const noexcept;

		private:
			/** @brief Internal string storage */
			std::string m_value;
		};

		//----------------------------------------------
		// Value::DateTime class
		//----------------------------------------------

		/**
		 * @brief Date and time value wrapper for format data type validation
		 */
		class DateTime final
		{
		public:
			/**
			 * @brief Construct date time value from DateTime
			 * @param value The date time value to convert to DateTimeOffset
			 */
			inline explicit DateTime( const nfx::time::DateTime& value ) noexcept;

			/**
			 * @brief Construct date time value from DateTimeOffset
			 * @param value The date time offset value to store
			 */
			inline explicit DateTime( const nfx::time::DateTimeOffset& value ) noexcept;

			/**
			 * @brief Construct date time value from string
			 * @param value The date time string to parse (e.g., "2024-03-15" or "2024-03-15T00:00:00Z")
			 * @throws std::runtime_error if string format is invalid
			 */
			inline explicit DateTime( std::string_view value );

			/**
			 * @brief Get the date and time value
			 * @return Reference to the internal date and time value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const nfx::time::DateTimeOffset& value() const noexcept;

		private:
			/** @brief Internal date and time storage */
			nfx::time::DateTimeOffset m_value;
		};

		//----------------------------------------------
		// Construction from value types
		//----------------------------------------------

		inline Value() noexcept;

		//-----------------------------
		// Converting constructors
		//-----------------------------

		/**
		 * @brief Converting constructors with optimal move semantics
		 * @details These constructors use the "pass-by-value then move" idiom for optimal performance:
		 * - When called with lvalue: parameter is copy-constructed, then moved into variant
		 * - When called with rvalue: parameter is move-constructed, then moved into variant
		 * - Avoids constructor ambiguity that would arise with separate copy/move overloads
		 * - Provides single, efficient path for both copy and move scenarios
		 * - Implementation uses std::move() to forward into std::variant storage
		 *
		 * Performance benefits over traditional copy/move constructor pairs:
		 * - No ambiguous overload resolution
		 * - Optimal performance for both lvalue and rvalue arguments
		 * - Simpler API surface with single constructor per type
		 * - Perfect forwarding semantics without template complexity
		 */

		/**
		 * @brief Construct Value from Decimal
		 * @param decimal The high-precision decimal value to store
		 */
		inline Value( Decimal decimal ) noexcept;

		/**
		 * @brief Construct Value from Integer
		 * @param integer The integer value to store
		 */
		inline Value( Integer integer ) noexcept;

		/**
		 * @brief Construct Value from Boolean
		 * @param boolean The boolean value to store
		 */
		inline Value( Boolean boolean ) noexcept;

		/**
		 * @brief Construct Value from String
		 * @param string The string value to store
		 */
		inline Value( String string ) noexcept;

		/**
		 * @brief Construct Value from DateTime
		 * @param dateTime The date and time value to store
		 */
		inline Value( DateTime dateTime ) noexcept;

		//----------------------------------------------
		// Type checking
		//----------------------------------------------

		/**
		 * @brief Check if value is a high-precision decimal
		 * @return True if the value holds a decimal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isDecimal() const noexcept;

		/**
		 * @brief Check if value is an integer
		 * @return True if the value holds an integer, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isInteger() const noexcept;

		/**
		 * @brief Check if value is a boolean
		 * @return True if the value holds a boolean, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isBoolean() const noexcept;

		/**
		 * @brief Check if value is a string
		 * @return True if the value holds a string, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isString() const noexcept;

		/**
		 * @brief Check if value is a date and time
		 * @return True if the value holds a datetime, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isDateTime() const noexcept;

		//----------------------------------------------
		// Value access
		//----------------------------------------------

		/**
		 * @brief Get the high-precision decimal value
		 * @return Reference to the decimal value
		 * @throws std::bad_variant_access if value is not a decimal
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Decimal& decimal() const;

		/**
		 * @brief Get the integer value
		 * @return Reference to the integer value
		 * @throws std::bad_variant_access if value is not an integer
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Integer& integer() const;

		/**
		 * @brief Get the boolean value
		 * @return Reference to the boolean value
		 * @throws std::bad_variant_access if value is not a boolean
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Boolean& boolean() const;

		/**
		 * @brief Get the string value
		 * @return Reference to the string value
		 * @throws std::bad_variant_access if value is not a string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const String& string() const;

		/**
		 * @brief Get the date and time value
		 * @return Reference to the datetime value
		 * @throws std::bad_variant_access if value is not a datetime
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const DateTime& dateTime() const;

		//----------------------------------------------
		// Variant index access
		//----------------------------------------------

		/**
		 * @brief Get the variant index for switch statements
		 * @return Index of the currently held type in the variant
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::size_t index() const noexcept;

		/**
		 * @brief Get the type as an enum for readable switch statements
		 * @return Type enum representing the currently held type
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Type type() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Variant storing the typed value */
		std::variant<Decimal, Integer, Boolean, String, DateTime> m_value{ String{ std::string_view{ "" } } };
	};

	//=====================================================================
	// DataChannelTypeName class
	//=====================================================================

	/** @brief Single data channel type name with description */
	class DataChannelTypeName final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor
		 * @param type Type name
		 * @param description Description
		 */
		inline explicit DataChannelTypeName( std::string_view type, std::string_view description ) noexcept;

		/** @brief Default constructor */
		DataChannelTypeName() = default;

		/** @brief Copy constructor */
		DataChannelTypeName( const DataChannelTypeName& ) = default;

		/** @brief Move constructor */
		DataChannelTypeName( DataChannelTypeName&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DataChannelTypeName() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment
		 * @param other The DataChannelTypeName to copy from
		 * @return Reference to this DataChannelTypeName
		 */
		DataChannelTypeName& operator=( const DataChannelTypeName& other ) = default;

		/**
		 * @brief Move assignment
		 * @param other The DataChannelTypeName to move from
		 * @return Reference to this DataChannelTypeName
		 */
		DataChannelTypeName& operator=( DataChannelTypeName&& other ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get type name
		 * @return Type name
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& type() const noexcept;

		/**
		 * @brief Get description
		 * @return Description
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& description() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Type name identifier */
		std::string m_type;

		/** @brief Human-readable description */
		std::string m_description;
	};

	//=====================================================================
	// DataChannelTypeNames class
	//=====================================================================

	/** @brief Collection of data channel type names with parsing capability */
	class DataChannelTypeNames final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with values
		 * @param values Collection of data channel type names
		 */
		inline explicit DataChannelTypeNames( std::vector<DataChannelTypeName> values ) noexcept;

		/** @brief Default constructor */
		DataChannelTypeNames() = default;

		/** @brief Copy constructor */
		DataChannelTypeNames( const DataChannelTypeNames& ) = default;

		/** @brief Move constructor */
		DataChannelTypeNames( DataChannelTypeNames&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DataChannelTypeNames() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment
		 * @param other The DataChannelTypeNames to copy from
		 * @return Reference to this DataChannelTypeNames
		 */
		DataChannelTypeNames& operator=( const DataChannelTypeNames& other ) = default;

		/**
		 * @brief Move assignment
		 * @param other The DataChannelTypeNames to move from
		 * @return Reference to this DataChannelTypeNames
		 */
		DataChannelTypeNames& operator=( DataChannelTypeNames&& other ) noexcept = default;

		//----------------------------------------------
		// DataChannelTypeNames::ParseResult class
		//----------------------------------------------

		/** @brief Result of parsing operation */
		class ParseResult
		{
		public:
			//----------------------------------------------
			// DataChannelTypeNames::ParseResult::Ok class
			//----------------------------------------------

			/**
			 * @brief Successful parse result containing a data channel type name
			 */
			class Ok final
			{
			public:
				/**
				 * @brief Construct successful parse result
				 * @param typeName The parsed data channel type name
				 */
				inline explicit Ok( DataChannelTypeName typeName ) noexcept;

				/**
				 * @brief Get the parsed data channel type name
				 * @return Reference to the data channel type name
				 * @note This function is marked [[nodiscard]] - the return value should not be ignored
				 */
				[[nodiscard]] inline const DataChannelTypeName& typeName() const noexcept;

			private:
				DataChannelTypeName m_typeName;
			};

			//----------------------------------------------
			// DataChannelTypeNames::ParseResult::Invalid class
			//----------------------------------------------

			/**
			 * @brief Failed parse result indicating invalid data channel type name
			 */
			class Invalid final
			{
			public:
				Invalid() = default;
			};

			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/**
			 * @brief Construct parse result from successful result
			 * @param ok The successful parse result
			 */
			inline ParseResult( Ok ok ) noexcept;

			/**
			 * @brief Construct parse result from failed result
			 * @param invalid The failed parse result
			 */
			inline ParseResult( Invalid invalid ) noexcept;

			//----------------------------------------------
			// Type checking
			//----------------------------------------------

			/**
			 * @brief Check if parse result is successful
			 * @return True if parse was successful, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool isOk() const noexcept;

			/**
			 * @brief Check if parse result is invalid
			 * @return True if parse failed, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool isInvalid() const noexcept;

			//----------------------------------------------
			// Value access
			//----------------------------------------------

			/**
			 * @brief Get the successful parse result
			 * @return Reference to the successful result
			 * @throws std::bad_variant_access if result is not successful
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const Ok& ok() const;

			/**
			 * @brief Get the failed parse result
			 * @return Reference to the failed result
			 * @throws std::bad_variant_access if result is not failed
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const Invalid& invalid() const;

		private:
			/** @brief Variant storage for parse result types */
			std::variant<Ok, Invalid> m_value;
		};

		//----------------------------------------------
		// DataChannelTypeNames::Iterator support
		//----------------------------------------------

		/** @brief Iterator type for iterating over data channel type names */
		using iterator = std::vector<DataChannelTypeName>::const_iterator;

		/** @brief Const iterator type for iterating over data channel type names */
		using const_iterator = std::vector<DataChannelTypeName>::const_iterator;

		//----------------------------------------------
		// Parsing
		//----------------------------------------------

		/**
		 * @brief Parse type name from string
		 * @param type Type string to parse
		 * @return Parse result
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] ParseResult parse( std::string_view type ) const;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Get iterator to beginning of data channel type names
		 * @return Iterator pointing to the first data channel type name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline iterator begin() const noexcept;

		/**
		 * @brief Get iterator to end of data channel type names
		 * @return Iterator pointing past the last data channel type name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline iterator end() const noexcept;

		/**
		 * @brief Get const iterator to beginning of data channel type names
		 * @return Const iterator pointing to the first data channel type name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const_iterator cbegin() const noexcept;

		/**
		 * @brief Get const iterator to end of data channel type names
		 * @return Const iterator pointing past the last data channel type name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const_iterator cend() const noexcept;

	private:
		std::vector<DataChannelTypeName> m_values;
	};

	//=====================================================================
	// FormatDataType class
	//=====================================================================

	/** @brief Single format data type with validation capability */
	class FormatDataType final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor
		 * @param type Type name
		 * @param description Description
		 */
		inline explicit FormatDataType( std::string_view type, std::string_view description ) noexcept;

		/** @brief Default constructor */
		FormatDataType() = default;

		/** @brief Copy constructor */
		FormatDataType( const FormatDataType& ) = default;

		/** @brief Move constructor */
		FormatDataType( FormatDataType&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~FormatDataType() = default;

		//-----------------------------
		// Assignment operators
		//-----------------------------

		/**
		 * @brief Copy assignment
		 * @param other The FormatDataType to copy from
		 * @return Reference to this FormatDataType
		 */
		FormatDataType& operator=( const FormatDataType& other ) = default;

		/**
		 * @brief Move assignment
		 * @param other The FormatDataType to move from
		 * @return Reference to this FormatDataType
		 */
		FormatDataType& operator=( FormatDataType&& other ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get type name
		 * @return Type name
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& type() const noexcept;

		/**
		 * @brief Get description
		 * @return Description
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& description() const noexcept;

		//----------------------------------------------
		// Validation
		//----------------------------------------------

		/**
		 * @brief Validate value according to format type
		 * @param value String value to validate
		 * @param outValue Output typed value
		 * @return Validation result
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] ValidateResult validate( std::string_view value, Value& outValue ) const;

		//----------------------------------------------
		// Pattern matching methods
		//----------------------------------------------

		/**
		 * @brief Action-based pattern matching on validated value
		 * @param value String value to validate and switch on
		 * @param onDecimal Action for decimal values
		 * @param onInteger Action for integer values
		 * @param onBoolean Action for boolean values
		 * @param onString Action for string values
		 * @param onDateTime Action for datetime values
		 * @throws ValidationException if validation fails
		 */
		template <typename DecimalFunc, typename IntegerFunc, typename BooleanFunc, typename StringFunc, typename DateTimeFunc>
		inline void switchOn( std::string_view value, DecimalFunc&& onDecimal, IntegerFunc&& onInteger, BooleanFunc&& onBoolean, StringFunc&& onString,
			DateTimeFunc&& onDateTime ) const;

		/**
		 * @brief Function-based pattern matching on validated value with return value
		 * @tparam T Return type
		 * @param value String value to validate and match on
		 * @param onDecimal Function for decimal values
		 * @param onInteger Function for integer values
		 * @param onBoolean Function for boolean values
		 * @param onString Function for string values
		 * @param onDateTime Function for datetime values
		 * @return Result of matched function
		 * @throws ValidationException if validation fails
		 */
		template <typename T, typename DecimalFunc, typename IntegerFunc, typename BooleanFunc, typename StringFunc, typename DateTimeFunc>
		inline T matchOn( std::string_view value, DecimalFunc&& onDecimal, IntegerFunc&& onInteger, BooleanFunc&& onBoolean, StringFunc&& onString,
			DateTimeFunc&& onDateTime ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Format data type name identifier */
		std::string m_type;

		/** @brief Human-readable description of the format data type */
		std::string m_description;
	};

	//=====================================================================
	// FormatDataTypes class
	//=====================================================================

	/**
	 * @brief Collection of format data types with parsing capability
	 */
	class FormatDataTypes final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with values
		 * @param values Collection of format data types
		 */
		inline explicit FormatDataTypes( std::vector<FormatDataType> values ) noexcept;

		/** @brief Default constructor */
		FormatDataTypes() = default;

		/** @brief Copy constructor */
		FormatDataTypes( const FormatDataTypes& ) = default;

		/** @brief Move constructor */
		FormatDataTypes( FormatDataTypes&& ) noexcept = default;

		/**
		 * @brief Copy assignment
		 * @param other The FormatDataTypes to copy from
		 * @return Reference to this FormatDataTypes
		 */
		FormatDataTypes& operator=( const FormatDataTypes& other ) = default;

		/**
		 * @brief Move assignment
		 * @param other The FormatDataTypes to move from
		 * @return Reference to this FormatDataTypes
		 */
		FormatDataTypes& operator=( FormatDataTypes&& other ) noexcept = default;

		//----------------------------------------------
		// FormatDataType::ParseResult class
		//----------------------------------------------

		/** @brief Result of parsing operation */
		class ParseResult
		{
		public:
			/** @brief Successful parse result */
			class Ok final
			{
			public:
				/**
				 * @brief Construct successful parse result
				 * @param typeName The parsed format data type
				 */
				inline explicit Ok( FormatDataType typeName ) noexcept;

				/**
				 * @brief Get the parsed format data type
				 * @return Reference to the format data type
				 * @note This function is marked [[nodiscard]] - the return value should not be ignored
				 */
				[[nodiscard]] inline const FormatDataType& typeName() const noexcept;

			private:
				FormatDataType m_typeName;
			};

			/** @brief Failed parse result */
			class Invalid final
			{
			public:
				/**
				 * @brief Default constructor for failed parse result
				 */
				Invalid() = default;
			};

			//-----------------------------
			// Construction from result
			//-----------------------------

			/**
			 * @brief Construct parse result from successful result
			 * @param ok The successful parse result
			 */
			inline ParseResult( Ok ok ) noexcept;

			/**
			 * @brief Construct parse result from failed result
			 * @param invalid The failed parse result
			 */
			inline ParseResult( Invalid invalid ) noexcept;

			//-----------------------------
			// Type checking
			//-----------------------------

			/**
			 * @brief Check if parse result is successful
			 * @return True if parse was successful, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool isOk() const noexcept;

			/**
			 * @brief Check if parse result is invalid
			 * @return True if parse failed, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool isInvalid() const noexcept;

			//-----------------------------
			// Value access
			//-----------------------------

			/**
			 * @brief Get the successful parse result
			 * @return Reference to the successful result
			 * @throws std::bad_variant_access if result is not successful
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const Ok& ok() const;

			/**
			 * @brief Get the failed parse result
			 * @return Reference to the failed result
			 * @throws std::bad_variant_access if result is not failed
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const Invalid& invalid() const;

		private:
			/** @brief Variant storage for parse result types */
			std::variant<Ok, Invalid> m_value;
		};

		//----------------------------------------------
		// FormatDataTypes::Iterator support
		//----------------------------------------------

		/** @brief Iterator type for iterating over format data types */
		using iterator = std::vector<FormatDataType>::const_iterator;

		/** @brief Const iterator type for iterating over format data types */
		using const_iterator = std::vector<FormatDataType>::const_iterator;

		//----------------------------------------------
		// Parsing
		//----------------------------------------------

		/**
		 * @brief Parse type name from string
		 * @param type Type string to parse
		 * @return Parse result
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] ParseResult parse( std::string_view type ) const;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Get iterator to beginning of format data types
		 * @return Iterator pointing to the first format data type
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline iterator begin() const noexcept;

		/**
		 * @brief Get iterator to end of format data types
		 * @return Iterator pointing past the last format data type
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline iterator end() const noexcept;

		/**
		 * @brief Get const iterator to beginning of format data types
		 * @return Const iterator pointing to the first format data type
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const_iterator cbegin() const noexcept;

		/**
		 * @brief Get const iterator to end of format data types
		 * @return Const iterator pointing past the last format data type
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const_iterator cend() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Collection of format data types */
		std::vector<FormatDataType> m_values;
	};

	//=====================================================================
	// ISO19848 class
	//=====================================================================

	/** @brief ISO 19848 standard data access */
	class ISO19848 final
	{
	public:
		//----------------------------------------------
		// Singleton access
		//----------------------------------------------

		/**
		 * @brief Get singleton instance
		 * @return Reference to singleton instance
		 */
		[[nodiscard]] static ISO19848& instance() noexcept;

		//----------------------------------------------
		// Public interface
		//----------------------------------------------

		/**
		 * @brief Get data channel type names for specified version
		 * @param version ISO 19848 version
		 * @return Data channel type names collection
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const DataChannelTypeNames& dataChannelTypeNames( ISO19848Version version );

		/**
		 * @brief Get format data types for specified version
		 * @param version ISO 19848 version
		 * @return Format data types collection
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const FormatDataTypes& formatDataTypes( ISO19848Version version );

	private:
		//----------------------------------------------
		// Singleton construction
		//----------------------------------------------

		/** @brief Default constructor. */
		ISO19848() noexcept;

		/** @brief Copy constructor */
		ISO19848( const ISO19848& ) = delete;

		/** @brief Move constructor */
		ISO19848( ISO19848&& ) = delete;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ISO19848() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		ISO19848& operator=( const ISO19848& ) = delete;

		/** @brief Move assignment operator */
		ISO19848& operator=( ISO19848&& ) = delete;
	};
} // namespace dnv::vista::sdk::transport

#include "dnv/vista/sdk/detail/transport/ISO19848.inl"
