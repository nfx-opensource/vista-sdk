/**
 * @file UniversalId.inl
 * @brief Inline implementations for LocalIdBuilder class
 * @details Provides inline implementations for simple accessor methods.
 */

#include "ISO19848AnnexC.h"

namespace dnv::vista::sdk
{
    inline bool UniversalId::operator==( const UniversalId& other ) const noexcept
    {
        return m_builder == other.m_builder;
    }

    inline bool UniversalId::operator!=( const UniversalId& other ) const noexcept
    {
        return !( *this == other );
    }

    inline constexpr std::string_view UniversalId::namingEntity() noexcept
    {
        return iso19848::annexC::NamingEntity;
    }

    inline const ImoNumber& UniversalId::imoNumber() const noexcept
    {
        return *m_builder.imoNumber();
    }

    inline const LocalId& UniversalId::localId() const noexcept
    {
        return m_localId;
    }

    inline const UniversalIdBuilder& UniversalId::builder() const noexcept
    {
        return m_builder;
    }

    inline std::string UniversalId::toString() const&
    {
        return m_builder.toString();
    }

    inline std::string UniversalId::toString() &&
    {
        return std::move( m_builder ).toString();
    }

    inline void UniversalId::toString( StringBuilder& sb ) const
    {
        m_builder.toString( sb );
    }
} // namespace dnv::vista::sdk
