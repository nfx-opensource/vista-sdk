/**
 * @file LocationBuilder.inl
 * @brief Inline implementations for LocationBuilder class
 * @details Provides inline implementations for simple accessor methods.
 */

namespace dnv::vista::sdk
{
    inline LocationBuilder::LocationBuilder(
        VisVersion visVersion, const std::unordered_map<char, LocationGroup>* reversedGroups )
        : m_visVersion{ visVersion },
          m_reversedGroups{ reversedGroups }
    {
    }

    inline LocationBuilder LocationBuilder::create( const Locations& locations )
    {
        return LocationBuilder{ locations.version(), &locations.m_reversedGroups };
    }

    inline VisVersion LocationBuilder::version() const noexcept
    {
        return m_visVersion;
    }

    inline std::optional<int> LocationBuilder::number() const noexcept
    {
        return m_number;
    }

    inline std::optional<char> LocationBuilder::side() const noexcept
    {
        return m_side;
    }

    inline std::optional<char> LocationBuilder::vertical() const noexcept
    {
        return m_vertical;
    }

    inline std::optional<char> LocationBuilder::transverse() const noexcept
    {
        return m_transverse;
    }

    inline std::optional<char> LocationBuilder::longitudinal() const noexcept
    {
        return m_longitudinal;
    }

    inline LocationBuilder LocationBuilder::withLocation( const std::optional<Location>& location ) const&
    {
        if( !location.has_value() )
        {
            throw std::invalid_argument{ "Location is empty" };
        }
        return withLocation( location.value() );
    }

    inline LocationBuilder LocationBuilder::withLocation( const std::optional<Location>& location ) &&
    {
        if( !location.has_value() )
        {
            throw std::invalid_argument{ "Location is empty" };
        }
        return std::move( *this ).withLocation( location.value() );
    }

    inline LocationBuilder LocationBuilder::withNumber( int number ) const&
    {
        if( number < 1 )
        {
            throw std::invalid_argument{ "Value should be greater than 0" };
        }

        LocationBuilder result = *this;
        result.m_number = number;
        return result;
    }

    inline LocationBuilder LocationBuilder::withNumber( int number ) &&
    {
        if( number < 1 )
        {
            throw std::invalid_argument{ "Value should be greater than 0" };
        }

        m_number = number;
        return std::move( *this );
    }

    inline LocationBuilder LocationBuilder::withCode( char code ) const&
    {
        // Find which group this character belongs to
        auto it = m_reversedGroups->find( code );
        if( it == m_reversedGroups->end() )
        {
            throw std::invalid_argument{ std::string{ "The value " } + code + " is an invalid Locations value" };
        }

        // Delegate to the appropriate with* method based on the group
        LocationGroup group = it->second;
        switch( group )
        {
            case LocationGroup::Side:
            {
                return withSide( code );
            }
            case LocationGroup::Vertical:
            {
                return withVertical( code );
            }
            case LocationGroup::Transverse:
            {
                return withTransverse( code );
            }
            case LocationGroup::Longitudinal:
            {
                return withLongitudinal( code );
            }
            default:
            {
                throw std::invalid_argument{ "Unsupported LocationGroup" };
            }
        }
    }

    inline LocationBuilder LocationBuilder::withCode( char code ) &&
    {
        // Find which group this character belongs to
        auto it = m_reversedGroups->find( code );
        if( it == m_reversedGroups->end() )
        {
            throw std::invalid_argument{ std::string{ "The value " } + code + " is an invalid Locations value" };
        }

        // Delegate to the appropriate with* method based on the group
        LocationGroup group = it->second;
        switch( group )
        {
            case LocationGroup::Side:
            {
                return std::move( *this ).withSide( code );
            }
            case LocationGroup::Vertical:
            {
                return std::move( *this ).withVertical( code );
            }
            case LocationGroup::Transverse:
            {
                return std::move( *this ).withTransverse( code );
            }
            case LocationGroup::Longitudinal:
            {
                return std::move( *this ).withLongitudinal( code );
            }
            default:
            {
                throw std::invalid_argument{ "Unsupported LocationGroup" };
            }
        }
    }

    inline LocationBuilder LocationBuilder::withSide( char side ) const&
    {
        // Validate that the character is valid for Side group
        auto it = m_reversedGroups->find( side );
        if( it == m_reversedGroups->end() || it->second != LocationGroup::Side )
        {
            throw std::invalid_argument{ std::string{ "The value " } + side + " is an invalid Side value" };
        }

        LocationBuilder result = *this;
        result.m_side = side;
        return result;
    }

    inline LocationBuilder LocationBuilder::withSide( char side ) &&
    {
        // Validate that the character is valid for Side group
        auto it = m_reversedGroups->find( side );
        if( it == m_reversedGroups->end() || it->second != LocationGroup::Side )
        {
            throw std::invalid_argument{ std::string{ "The value " } + side + " is an invalid Side value" };
        }

        m_side = side;
        return std::move( *this );
    }

    inline LocationBuilder LocationBuilder::withVertical( char vertical ) const&
    {
        // Validate that the character is valid for Vertical group
        auto it = m_reversedGroups->find( vertical );
        if( it == m_reversedGroups->end() || it->second != LocationGroup::Vertical )
        {
            throw std::invalid_argument{ std::string{ "The value " } + vertical + " is an invalid Vertical value" };
        }

        LocationBuilder result = *this;
        result.m_vertical = vertical;
        return result;
    }

    inline LocationBuilder LocationBuilder::withVertical( char vertical ) &&
    {
        // Validate that the character is valid for Vertical group
        auto it = m_reversedGroups->find( vertical );
        if( it == m_reversedGroups->end() || it->second != LocationGroup::Vertical )
        {
            throw std::invalid_argument{ std::string{ "The value " } + vertical + " is an invalid Vertical value" };
        }

        m_vertical = vertical;
        return std::move( *this );
    }

    inline LocationBuilder LocationBuilder::withTransverse( char transverse ) const&
    {
        // Validate that the character is valid for Transverse group
        auto it = m_reversedGroups->find( transverse );
        if( it == m_reversedGroups->end() || it->second != LocationGroup::Transverse )
        {
            throw std::invalid_argument{ std::string{ "The value " } + transverse + " is an invalid Transverse value" };
        }

        LocationBuilder result = *this;
        result.m_transverse = transverse;
        return result;
    }

    inline LocationBuilder LocationBuilder::withTransverse( char transverse ) &&
    {
        // Validate that the character is valid for Transverse group
        auto it = m_reversedGroups->find( transverse );
        if( it == m_reversedGroups->end() || it->second != LocationGroup::Transverse )
        {
            throw std::invalid_argument{ std::string{ "The value " } + transverse + " is an invalid Transverse value" };
        }

        m_transverse = transverse;
        return std::move( *this );
    }

    inline LocationBuilder LocationBuilder::withLongitudinal( char longitudinal ) const&
    {
        // Validate that the character is valid for Longitudinal group
        auto it = m_reversedGroups->find( longitudinal );
        if( it == m_reversedGroups->end() || it->second != LocationGroup::Longitudinal )
        {
            throw std::invalid_argument{ std::string{ "The value " } + longitudinal +
                                         " is an invalid Longitudinal value" };
        }

        LocationBuilder result = *this;
        result.m_longitudinal = longitudinal;
        return result;
    }

    inline LocationBuilder LocationBuilder::withLongitudinal( char longitudinal ) &&
    {
        // Validate that the character is valid for Longitudinal group
        auto it = m_reversedGroups->find( longitudinal );
        if( it == m_reversedGroups->end() || it->second != LocationGroup::Longitudinal )
        {
            throw std::invalid_argument{ std::string{ "The value " } + longitudinal +
                                         " is an invalid Longitudinal value" };
        }

        m_longitudinal = longitudinal;
        return std::move( *this );
    }

    inline LocationBuilder LocationBuilder::withoutNumber() const&
    {
        return withoutValue( LocationGroup::Number );
    }

    inline LocationBuilder LocationBuilder::withoutNumber() &&
    {
        return std::move( *this ).withoutValue( LocationGroup::Number );
    }

    inline LocationBuilder LocationBuilder::withoutSide() const&
    {
        return withoutValue( LocationGroup::Side );
    }

    inline LocationBuilder LocationBuilder::withoutSide() &&
    {
        return std::move( *this ).withoutValue( LocationGroup::Side );
    }

    inline LocationBuilder LocationBuilder::withoutVertical() const&
    {
        return withoutValue( LocationGroup::Vertical );
    }

    inline LocationBuilder LocationBuilder::withoutVertical() &&
    {
        return std::move( *this ).withoutValue( LocationGroup::Vertical );
    }

    inline LocationBuilder LocationBuilder::withoutTransverse() const&
    {
        return withoutValue( LocationGroup::Transverse );
    }

    inline LocationBuilder LocationBuilder::withoutTransverse() &&
    {
        return std::move( *this ).withoutValue( LocationGroup::Transverse );
    }

    inline LocationBuilder LocationBuilder::withoutLongitudinal() const&
    {
        return withoutValue( LocationGroup::Longitudinal );
    }

    inline LocationBuilder LocationBuilder::withoutLongitudinal() &&
    {
        return std::move( *this ).withoutValue( LocationGroup::Longitudinal );
    }

    inline LocationBuilder LocationBuilder::withoutValue( LocationGroup group ) const&
    {
        LocationBuilder result = *this;

        switch( group )
        {
            case LocationGroup::Number:
            {
                result.m_number = std::nullopt;
                break;
            }
            case LocationGroup::Side:
            {
                result.m_side = std::nullopt;
                break;
            }
            case LocationGroup::Vertical:
            {
                result.m_vertical = std::nullopt;
                break;
            }
            case LocationGroup::Transverse:
            {
                result.m_transverse = std::nullopt;
                break;
            }
            case LocationGroup::Longitudinal:
            {
                result.m_longitudinal = std::nullopt;
                break;
            }
            default:
            {
                throw std::invalid_argument{ "Unsupported LocationGroup" };
            }
        }

        return result;
    }

    inline LocationBuilder LocationBuilder::withoutValue( LocationGroup group ) &&
    {
        switch( group )
        {
            case LocationGroup::Number:
            {
                m_number = std::nullopt;
                break;
            }
            case LocationGroup::Side:
            {
                m_side = std::nullopt;
                break;
            }
            case LocationGroup::Vertical:
            {
                m_vertical = std::nullopt;
                break;
            }
            case LocationGroup::Transverse:
            {
                m_transverse = std::nullopt;
                break;
            }
            case LocationGroup::Longitudinal:
            {
                m_longitudinal = std::nullopt;
                break;
            }
            default:
            {
                throw std::invalid_argument{ "Unsupported LocationGroup" };
            }
        }

        return std::move( *this );
    }

    inline Location LocationBuilder::build() const
    {
        return Location{ toString() };
    }

    inline std::string LocationBuilder::toString() const
    {
        StringBuilder sb;

        if( m_side.has_value() )
        {
            sb << m_side.value();
        }
        if( m_vertical.has_value() )
        {
            sb << m_vertical.value();
        }
        if( m_transverse.has_value() )
        {
            sb << m_transverse.value();
        }
        if( m_longitudinal.has_value() )
        {
            sb << m_longitudinal.value();
        }

        // Sort alphabetically
        std::sort( sb.begin(), sb.end() );

        if( m_number.has_value() )
        {
            sb.prepend( m_number.value() );
        }

        return sb.toString();
    }
} // namespace dnv::vista::sdk
