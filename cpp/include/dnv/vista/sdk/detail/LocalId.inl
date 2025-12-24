/**
 * @file LocalId.inl
 * @brief Inline implementations for LocalId class
 * @details Provides inline implementations for LocalId accessor methods.
 *          All accessors delegate to the internal builder.
 */

#include "ISO19848AnnexC.h"

namespace dnv::vista::sdk
{
    inline bool LocalId::operator==( const LocalId& other ) const noexcept
    {
        return m_builder == other.m_builder;
    }

    inline bool LocalId::operator!=( const LocalId& other ) const noexcept
    {
        return m_builder != other.m_builder;
    }

    inline constexpr std::string_view LocalId::namingRule() noexcept
    {
        return iso19848::annexC::VersionedNamingRule;
    }

    inline VisVersion LocalId::version() const noexcept
    {
        return *m_builder.version();
    }

    inline bool LocalId::isVerboseMode() const noexcept
    {
        return m_builder.isVerboseMode();
    }

    inline const GmodPath& LocalId::primaryItem() const noexcept
    {
        return *m_builder.primaryItem();
    }

    inline const std::optional<GmodPath>& LocalId::secondaryItem() const noexcept
    {
        return m_builder.secondaryItem();
    }

    inline const std::optional<MetadataTag>& LocalId::quantity() const noexcept
    {
        return m_builder.quantity();
    }

    inline const std::optional<MetadataTag>& LocalId::content() const noexcept
    {
        return m_builder.content();
    }

    inline const std::optional<MetadataTag>& LocalId::calculation() const noexcept
    {
        return m_builder.calculation();
    }

    inline const std::optional<MetadataTag>& LocalId::state() const noexcept
    {
        return m_builder.state();
    }

    inline const std::optional<MetadataTag>& LocalId::command() const noexcept
    {
        return m_builder.command();
    }

    inline const std::optional<MetadataTag>& LocalId::type() const noexcept
    {
        return m_builder.type();
    }

    inline const std::optional<MetadataTag>& LocalId::position() const noexcept
    {
        return m_builder.position();
    }

    inline const std::optional<MetadataTag>& LocalId::detail() const noexcept
    {
        return m_builder.detail();
    }

    inline std::vector<MetadataTag> LocalId::metadataTags() const
    {
        return m_builder.metadataTags();
    }

    inline bool LocalId::hasCustomTag() const noexcept
    {
        return m_builder.hasCustomTag();
    }

    inline const LocalIdBuilder& LocalId::builder() const noexcept
    {
        return m_builder;
    }

    inline std::string LocalId::toString() const&
    {
        return m_builder.toString();
    }

    inline std::string LocalId::toString() &&
    {
        return std::move( m_builder ).toString();
    }

    inline void LocalId::toString( StringBuilder& sb ) const
    {
        m_builder.toString( sb );
    }
} // namespace dnv::vista::sdk

namespace dnv::vista::sdk::mqtt
{
    inline std::string LocalId::toString() const&
    {
        StringBuilder sb;
        toString( sb );
        return sb.toString();
    }

    inline std::string LocalId::toString() &&
    {
        StringBuilder sb;
        toString( sb );
        return std::move( sb ).toString();
    }

    inline void LocalId::toString( StringBuilder& sb ) const
    {
        // Naming rule (NO leading slash for MQTT): "dnv-v2/"
        sb << "dnv-v2/";

        sb << "vis-" << VisVersions::toString( version() ) << "/";

        appendPrimaryItem( m_builder, sb );
        appendSecondaryItem( m_builder, sb );
        appendMeta( sb, quantity() );
        appendMeta( sb, content() );
        appendMeta( sb, calculation() );
        appendMeta( sb, state() );
        appendMeta( sb, command() );
        appendMeta( sb, type() );
        appendMeta( sb, position() );
        appendMeta( sb, detail() );

        if( sb.size() > 0 && sb[sb.size() - 1] == '/' )
        {
            sb.resize( sb.size() - 1 );
        }
    }

    inline void LocalId::appendPath( StringBuilder& builder, const GmodPath& path ) const
    {
        // Convert path to string with MQTT separator (underscore instead of slash)
        path.toString( builder, m_separator );
        builder << '/';
    }

    inline void LocalId::appendPrimaryItem( const LocalIdBuilder& localIdBuilder, StringBuilder& builder ) const
    {
        appendPath( builder, localIdBuilder.primaryItem().value() );
    }

    inline void LocalId::appendSecondaryItem( const LocalIdBuilder& localIdBuilder, StringBuilder& builder ) const
    {
        if( localIdBuilder.secondaryItem().has_value() )
        {
            appendPath( builder, localIdBuilder.secondaryItem().value() );
        }
        else
        {
            // No secondary item - use placeholder
            builder << "_/";
        }
    }

    inline void LocalId::appendMeta( StringBuilder& builder, const std::optional<MetadataTag>& tag ) const
    {
        if( !tag.has_value() )
        {
            builder << "_/";
        }
        else
        {
            tag.value().toString( builder );
        }
    }
} // namespace dnv::vista::sdk::mqtt
