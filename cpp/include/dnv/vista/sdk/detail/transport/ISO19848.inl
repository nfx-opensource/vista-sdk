/**
 * @file ISO19848.inl
 * @brief Inline implementations for ISO 19848 classes
 */

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// Value class
	//=====================================================================

	//----------------------------------------------
	// Construction from value types
	//----------------------------------------------

	inline Value::Value() noexcept
		: m_value{ String{ std::string_view{ "" } } }
	{
	}

	//-----------------------------
	// Converting constructors
	//-----------------------------

	inline Value::Value( Decimal decimal ) noexcept
		: m_value{ std::move( decimal ) }
	{
	}

	inline Value::Value( Integer integer ) noexcept
		: m_value{ std::move( integer ) }
	{
	}

	inline Value::Value( Boolean boolean ) noexcept
		: m_value{ std::move( boolean ) }
	{
	}

	inline Value::Value( String string ) noexcept
		: m_value{ std::move( string ) }
	{
	}

	inline Value::Value( DateTime dateTime ) noexcept
		: m_value{ std::move( dateTime ) }
	{
	}

	//----------------------------------------------
	// Value type constructors
	//----------------------------------------------

	inline Value::Decimal::Decimal( const nfx::datatypes::Decimal& value ) noexcept
		: m_value{ value }
	{
	}

	inline Value::Decimal::Decimal( double value ) noexcept
		: m_value{ value }
	{
	}

	inline Value::Integer::Integer( int value ) noexcept
		: m_value{ value }
	{
	}

	inline Value::Integer::Integer( int64_t value ) noexcept
		: m_value{ value }
	{
	}

	inline Value::Boolean::Boolean( bool value ) noexcept
		: m_value{ value }
	{
	}

	inline Value::String::String( std::string_view value ) noexcept
		: m_value{ value }
	{
	}

	inline Value::DateTime::DateTime( const nfx::time::DateTime& value ) noexcept
		: m_value{ nfx::time::DateTimeOffset{ value, nfx::time::TimeSpan() } }
	{
	}

	inline Value::DateTime::DateTime( const nfx::time::DateTimeOffset& value ) noexcept
		: m_value{ value }
	{
	}

	inline Value::DateTime::DateTime( std::string_view value )
		: m_value{ nfx::time::DateTimeOffset{ value } }
	{
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	inline bool Value::isDecimal() const noexcept
	{
		return std::holds_alternative<Decimal>( m_value );
	}

	inline bool Value::isInteger() const noexcept
	{
		return std::holds_alternative<Integer>( m_value );
	}

	inline bool Value::isBoolean() const noexcept
	{
		return std::holds_alternative<Boolean>( m_value );
	}

	inline bool Value::isString() const noexcept
	{
		return std::holds_alternative<String>( m_value );
	}

	inline bool Value::isDateTime() const noexcept
	{
		return std::holds_alternative<DateTime>( m_value );
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline const nfx::datatypes::Decimal& Value::Decimal::value() const noexcept
	{
		return m_value;
	}

	inline int64_t Value::Integer::value() const noexcept
	{
		return m_value;
	}

	inline bool Value::Boolean::value() const noexcept
	{
		return m_value;
	}

	inline const std::string& Value::String::value() const noexcept
	{
		return m_value;
	}

	inline const nfx::time::DateTimeOffset& Value::DateTime::value() const noexcept
	{
		return m_value;
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline const Value::Decimal& Value::decimal() const
	{
		return std::get<Decimal>( m_value );
	}

	inline const Value::Boolean& Value::boolean() const
	{
		return std::get<Boolean>( m_value );
	}

	inline const Value::Integer& Value::integer() const
	{
		return std::get<Integer>( m_value );
	}

	inline const Value::String& Value::string() const
	{
		return std::get<String>( m_value );
	}

	inline const Value::DateTime& Value::dateTime() const
	{
		return std::get<DateTime>( m_value );
	}

	//----------------------------------------------
	// Variant index access
	//----------------------------------------------

	inline std::size_t Value::index() const noexcept
	{
		return m_value.index();
	}

	inline Value::Type Value::type() const noexcept
	{
		return static_cast<Type>( m_value.index() );
	}

	//=====================================================================
	// DataChannelTypeName class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelTypeName::DataChannelTypeName( std::string_view type, std::string_view description ) noexcept
		: m_type{ type },
		  m_description{ description }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::string& DataChannelTypeName::type() const noexcept
	{
		return m_type;
	}

	inline const std::string& DataChannelTypeName::description() const noexcept
	{
		return m_description;
	}

	//=====================================================================
	// DataChannelTypeNames class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelTypeNames::DataChannelTypeNames( std::vector<DataChannelTypeName> values ) noexcept
		: m_values{ std::move( values ) }
	{
	}

	//----------------------------------------------
	// DataChannelTypeNames::ParseResult class
	//----------------------------------------------

	//-----------------------------
	// Construction
	//-----------------------------

	inline DataChannelTypeNames::ParseResult::Ok::Ok( DataChannelTypeName typeName ) noexcept
		: m_typeName{ std::move( typeName ) }
	{
	}

	inline const DataChannelTypeName& DataChannelTypeNames::ParseResult::Ok::typeName() const noexcept
	{
		return m_typeName;
	}

	inline DataChannelTypeNames::ParseResult::ParseResult( Ok ok ) noexcept
		: m_value{ std::move( ok ) }
	{
	}

	inline DataChannelTypeNames::ParseResult::ParseResult( Invalid invalid ) noexcept
		: m_value{ std::move( invalid ) }
	{
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	inline bool DataChannelTypeNames::ParseResult::isOk() const noexcept
	{
		return std::holds_alternative<Ok>( m_value );
	}

	inline bool DataChannelTypeNames::ParseResult::isInvalid() const noexcept
	{
		return std::holds_alternative<Invalid>( m_value );
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline const DataChannelTypeNames::ParseResult::Ok& DataChannelTypeNames::ParseResult::ok() const
	{
		return std::get<Ok>( m_value );
	}

	inline const DataChannelTypeNames::ParseResult::Invalid& DataChannelTypeNames::ParseResult::invalid() const
	{
		return std::get<Invalid>( m_value );
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	inline DataChannelTypeNames::iterator DataChannelTypeNames::begin() const noexcept
	{
		return m_values.begin();
	}

	inline DataChannelTypeNames::iterator DataChannelTypeNames::end() const noexcept
	{
		return m_values.end();
	}

	inline DataChannelTypeNames::const_iterator DataChannelTypeNames::cbegin() const noexcept
	{
		return m_values.cbegin();
	}

	inline DataChannelTypeNames::const_iterator DataChannelTypeNames::cend() const noexcept
	{
		return m_values.cend();
	}

	//=====================================================================
	// FormatDataType class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline FormatDataType::FormatDataType( std::string_view type, std::string_view description ) noexcept
		: m_type{ type },
		  m_description{ description }
	{
	}

	//=====================================================================
	// FormatDataTypes class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline FormatDataTypes::FormatDataTypes( std::vector<FormatDataType> values ) noexcept
		: m_values{ std::move( values ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::string& FormatDataType::type() const noexcept
	{
		return m_type;
	}

	inline const std::string& FormatDataType::description() const noexcept
	{
		return m_description;
	}

	//----------------------------------------------
	// FormatDataType::ParseResult class
	//----------------------------------------------

	inline FormatDataTypes::ParseResult::Ok::Ok( FormatDataType typeName ) noexcept
		: m_typeName{ std::move( typeName ) }
	{
	}

	inline const FormatDataType& FormatDataTypes::ParseResult::Ok::typeName() const noexcept
	{
		return m_typeName;
	}

	inline FormatDataTypes::ParseResult::ParseResult( Ok ok ) noexcept
		: m_value{ std::move( ok ) }
	{
	}

	inline FormatDataTypes::ParseResult::ParseResult( Invalid invalid ) noexcept
		: m_value{ std::move( invalid ) }
	{
	}

	inline bool FormatDataTypes::ParseResult::isOk() const noexcept
	{
		return std::holds_alternative<Ok>( m_value );
	}

	inline bool FormatDataTypes::ParseResult::isInvalid() const noexcept
	{
		return std::holds_alternative<Invalid>( m_value );
	}

	inline const FormatDataTypes::ParseResult::Ok& FormatDataTypes::ParseResult::ok() const
	{
		return std::get<Ok>( m_value );
	}

	inline const FormatDataTypes::ParseResult::Invalid& FormatDataTypes::ParseResult::invalid() const
	{
		return std::get<Invalid>( m_value );
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	inline FormatDataTypes::iterator FormatDataTypes::begin() const noexcept
	{
		return m_values.begin();
	}

	inline FormatDataTypes::iterator FormatDataTypes::end() const noexcept
	{
		return m_values.end();
	}

	inline FormatDataTypes::const_iterator FormatDataTypes::cbegin() const noexcept
	{
		return m_values.cbegin();
	}

	inline FormatDataTypes::const_iterator FormatDataTypes::cend() const noexcept
	{
		return m_values.cend();
	}

	//----------------------------------------------
	// Pattern matching methods
	//----------------------------------------------

	template <typename DecimalFunc, typename IntegerFunc, typename BooleanFunc, typename StringFunc, typename DateTimeFunc>
	inline void FormatDataType::switchOn(
		std::string_view value, DecimalFunc&& onDecimal, IntegerFunc&& onInteger,
		BooleanFunc&& onBoolean, StringFunc&& onString, DateTimeFunc&& onDateTime ) const
	{
		Value typedValue;
		auto result = validate( value, typedValue );

		if ( result.isInvalid() )
		{
			throw ValidationException{ "Invalid value" };
		}

		switch ( typedValue.type() )
		{
			case Value::Type::Decimal:
			{
				onDecimal( typedValue.decimal().value() );
				break;
			}
			case Value::Type::Integer:
			{
				onInteger( typedValue.integer().value() );
				break;
			}
			case Value::Type::Boolean:
			{
				onBoolean( typedValue.boolean().value() );
				break;
			}
			case Value::Type::String:
			{
				onString( typedValue.string().value() );
				break;
			}
			case Value::Type::DateTime:
			{
				onDateTime( typedValue.dateTime().value() );
				break;
			}
			default:
			{
				throw std::logic_error{ "Should never happen" };
			}
		}
	}

	template <typename T, typename DecimalFunc, typename IntegerFunc, typename BooleanFunc, typename StringFunc, typename DateTimeFunc>
	inline T FormatDataType::matchOn(
		std::string_view value, DecimalFunc&& onDecimal, IntegerFunc&& onInteger,
		BooleanFunc&& onBoolean, StringFunc&& onString, DateTimeFunc&& onDateTime ) const
	{
		Value typedValue;
		auto result = validate( value, typedValue );

		if ( result.isInvalid() )
		{
			throw ValidationException{ "Invalid value" };
		}

		switch ( typedValue.type() )
		{
			case Value::Type::Boolean:
			{
				return onBoolean( typedValue.boolean().value() );
			}
			case Value::Type::Integer:
			{
				return onInteger( typedValue.integer().value() );
			}
			case Value::Type::Decimal:
			{
				return onDecimal( typedValue.decimal().value() );
			}
			case Value::Type::String:
			{
				return onString( typedValue.string().value() );
			}
			case Value::Type::DateTime:
			{
				return onDateTime( typedValue.dateTime().value() );
			}
			default:
			{
				throw std::logic_error{ "Should never happen" };
			}
		}
	}
} // namespace dnv::vista::sdk::transport
