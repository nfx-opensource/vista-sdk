/**
 * @file Codebook.inl
 * @brief Inline implementations for Codebook class
 * @details Provides inline implementations for simple accessor methods.
 */

#include <nfx/StringUtils.h>

namespace dnv::vista::sdk
{
    inline CodebookName Codebook::name() const noexcept
    {
        return m_name;
    }

    inline const HashSet<std::string>& Codebook::standardValues() const noexcept
    {
        return m_standardValues;
    }

    inline const HashSet<std::string>& Codebook::groups() const noexcept
    {
        return m_groups;
    }

    inline bool Codebook::hasGroup( std::string_view group ) const noexcept
    {
        return m_groups.find( group ) != nullptr;
    }

    inline bool Codebook::hasStandardValue( std::string_view value ) const noexcept
    {
        if( m_name == CodebookName::Position && nfx::string::isAllDigits( value ) )
        {
            return true;
        }

        return m_standardValues.find( value ) != nullptr;
    }
} // namespace dnv::vista::sdk
