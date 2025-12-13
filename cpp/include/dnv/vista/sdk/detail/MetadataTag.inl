/**
 * @file MetadataTag.inl
 * @brief MetadataTag inline implementations
 * @details Inline method implementations for MetadataTag accessors
 */

namespace dnv::vista::sdk
{
    inline MetadataTag::MetadataTag( CodebookName name, std::string value, bool isCustom ) noexcept
        : m_name{ name },
          m_custom{ isCustom },
          m_value{ std::move( value ) }
    {
    }

    inline bool MetadataTag::operator==( const MetadataTag& other ) const noexcept
    {
        return m_name == other.m_name && m_value == other.m_value;
    }

    inline bool MetadataTag::operator!=( const MetadataTag& other ) const noexcept
    {
        return !( *this == other );
    }

    inline CodebookName MetadataTag::name() const noexcept
    {
        return m_name;
    }

    inline const std::string& MetadataTag::value() const noexcept
    {
        return m_value;
    }

    inline char MetadataTag::prefix() const noexcept
    {
        return m_custom ? '~' : '-';
    }

    inline bool MetadataTag::isCustom() const noexcept
    {
        return m_custom;
    }

    inline void MetadataTag::toString( StringBuilder& builder, char separator ) const
    {
        builder << CodebookNames::toPrefix( m_name ) << ( m_custom ? '~' : '-' ) << m_value << separator;
    }
} // namespace dnv::vista::sdk
