/**
 * @file LocalIdBuilder.cpp
 * @brief Implementation of VIS LocalId builder
 * @details Provides implementations for LocalIdBuilder class for building LocalId instances.
 */

#include "dnv/vista/sdk/LocalIdBuilder.h"

#include "internal/parsing/LocalIdParsingErrorBuilder.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/StringBuilder.h"
#include "dnv/vista/sdk/VIS.h"

#include <nfx/StringUtils.h>

#include <algorithm>
#include <cctype>

namespace dnv::vista::sdk
{
    namespace
    {
        using namespace dnv::vista::sdk;
        using namespace dnv::vista::sdk::internal;

        inline size_t findNext( std::string_view str, size_t pos, char ch )
        {
            auto idx = str.find( ch, pos );
            return idx == std::string_view::npos ? str.size() : idx;
        }

        inline std::string_view extractSegment( std::string_view str, size_t& pos )
        {
            if( pos >= str.size() )
            {
                return {};
            }

            size_t start = pos;
            size_t end = findNext( str, pos, '/' );
            pos = end;

            return str.substr( start, end - start );
        }

        inline void skipSlash( size_t& pos )
        {
            ++pos;
        }

        /**
         * @brief Normalize a common name for URI-safe verbose mode output
         * @details Converts to lowercase, replaces spaces with dots, keeps only URI-unreserved
         *          characters (RFC 3986: alphanumeric, -, ., _, ~), and collapses consecutive dots.
         *          This follows VIS LocalId conventions for verbose mode.
         * @param name The common name to normalize
         * @param builder StringBuilder to append the normalized name to
         */
        inline void appendNormalizedCommonName( std::string_view name, StringBuilder& builder )
        {
            // Collapse consecutive whitespace to single spaces
            auto collapsed = nfx::string::collapseWhitespace( name );

            // Keep only URI-unreserved characters, convert to lowercase, replace spaces with dots
            char prevChar = '\0';
            for( char ch : collapsed )
            {
                // Convert space to dot, otherwise lowercase
                char current = ( ch == ' ' ) ? '.' : nfx::string::toLower( ch );

                // Keep only URI-unreserved characters (alphanumeric, -, ., _, ~)
                if( !nfx::string::isUriUnreserved( current ) && current != '.' )
                {
                    continue;
                }

                // Skip consecutive dots
                if( current == '.' && prevChar == '.' )
                {
                    continue;
                }

                builder << std::string_view( &current, 1 );
                prevChar = current;
            }
        }

        bool parseNamingRule( std::string_view str, size_t& pos, LocalIdParsingErrorBuilder* errorBuilder )
        {
            auto segment = extractSegment( str, pos );

            if( segment.empty() )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError( LocalIdParsingState::NamingRule, "Missing naming rule" );
                }

                return false;
            }

            if( !nfx::string::equals( segment, LocalIdBuilder::namingRule() ) )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        LocalIdParsingState::NamingRule,
                        std::string{ "Invalid naming rule, expected 'dnv-v2', got '" } + std::string{ segment } + "'" );
                }

                return false;
            }

            skipSlash( pos );

            return true;
        }

        std::optional<VisVersion> parseVisVersion(
            std::string_view str, size_t& pos, LocalIdParsingErrorBuilder* errorBuilder )
        {
            auto segment = extractSegment( str, pos );

            if( segment.empty() )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError( LocalIdParsingState::VisVersion, "Missing VIS version" );
                }

                return std::nullopt;
            }

            if( !nfx::string::startsWith( segment, "vis-" ) )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        LocalIdParsingState::VisVersion,
                        std::string{ "Invalid VIS version format, expected 'vis-X-Ya', got '" } +
                            std::string{ segment } + "'" );
                }

                return std::nullopt;
            }

            auto versionStr = nfx::string::substringAfter( segment, "vis-" );
            VisVersion version;
            if( !VisVersions::fromString( versionStr, version ) )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        LocalIdParsingState::VisVersion,
                        std::string{ "Unknown VIS version: '" } + std::string{ versionStr } + "'" );
                }

                return std::nullopt;
            }

            skipSlash( pos );

            return version;
        }

        std::optional<GmodPath> parsePrimaryItem(
            std::string_view str,
            size_t& pos,
            const Gmod& gmod,
            const Locations& locations,
            LocalIdParsingErrorBuilder* errorBuilder )
        {
            // Find end of primary item (either /sec, /meta, or /~ for verbose mode)
            size_t start = pos;
            size_t pathEnd = pos;

            while( pathEnd < str.size() )
            {
                size_t nextSlash = findNext( str, pathEnd, '/' );
                if( nextSlash == str.size() )
                {
                    pathEnd = nextSlash;
                    break;
                }

                // Peek ahead to see if we hit /sec, /meta, or /~ (verbose mode)
                size_t peekStart = nextSlash + 1;
                if( peekStart < str.size() )
                {
                    auto remaining = str.substr( peekStart );

                    if( nfx::string::startsWith( remaining, "sec/" ) || nfx::string::startsWith( remaining, "sec" ) )
                    {
                        pathEnd = nextSlash;
                        break;
                    }
                    if( nfx::string::startsWith( remaining, "meta" ) )
                    {
                        pathEnd = nextSlash;
                        break;
                    }
                    if( str[peekStart] == '~' )
                    {
                        pathEnd = nextSlash;
                        break;
                    }
                }

                pathEnd = nextSlash + 1;
            }

            if( pathEnd == start )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError( LocalIdParsingState::PrimaryItem, "Missing primary item" );
                }

                return std::nullopt;
            }

            auto pathStr = str.substr( start, pathEnd - start );
            ParsingErrors parseErrors;
            auto path = GmodPath::fromShortPath( pathStr, gmod, locations, parseErrors );

            if( !path.has_value() )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        LocalIdParsingState::PrimaryItem,
                        std::string{ "Invalid Gmod path in primary item: '" } + std::string{ pathStr } + "'" );

                    // Include parse errors if available
                    if( parseErrors.hasErrors() )
                    {
                        for( const auto& [type, message] : parseErrors )
                        {
                            errorBuilder->addError( LocalIdParsingState::PrimaryItem, message );
                        }
                    }
                }
            }

            pos = pathEnd;
            if( pos < str.size() && str[pos] == '/' )
            {
                skipSlash( pos );
            }

            return path;
        }

        std::optional<GmodPath> parseSecondaryItem(
            std::string_view str,
            size_t& pos,
            const Gmod& gmod,
            const Locations& locations,
            LocalIdParsingErrorBuilder* errorBuilder )
        {
            // Expect "sec" keyword

            auto keyword = extractSegment( str, pos );
            if( !nfx::string::equals( keyword, "sec" ) )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        LocalIdParsingState::SecondaryItem,
                        std::string{ "Expected 'sec' keyword, got '" } + std::string{ keyword } + "'" );
                }

                return std::nullopt;
            }

            skipSlash( pos );

            // Find end of secondary item (/meta or /~ for verbose mode)
            size_t start = pos;
            size_t pathEnd = pos;

            while( pathEnd < str.size() )
            {
                size_t nextSlash = findNext( str, pathEnd, '/' );
                if( nextSlash == str.size() )
                {
                    pathEnd = nextSlash;
                    break;
                }

                // Check if we hit /meta or /~ (verbose mode)
                size_t peekStart = nextSlash + 1;
                if( peekStart < str.size() )
                {
                    auto remaining = str.substr( peekStart );

                    if( nfx::string::startsWith( remaining, "meta" ) )
                    {
                        pathEnd = nextSlash;
                        break;
                    }
                    if( str[peekStart] == '~' )
                    {
                        pathEnd = nextSlash;
                        break;
                    }
                }

                pathEnd = nextSlash + 1;
            }
            if( pathEnd == start )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError( LocalIdParsingState::SecondaryItem, "Missing secondary item after /sec/" );
                }

                return std::nullopt;
            }

            auto pathStr = str.substr( start, pathEnd - start );
            ParsingErrors parseErrors;
            auto path = GmodPath::fromShortPath( pathStr, gmod, locations, parseErrors );

            if( !path.has_value() )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        LocalIdParsingState::SecondaryItem,
                        std::string{ "Invalid Gmod path in secondary item: '" } + std::string{ pathStr } + "'" );
                }
            }

            pos = pathEnd;
            if( pos < str.size() && str[pos] == '/' )
            {
                skipSlash( pos );
            }

            return path;
        }

        std::optional<MetadataTag> parseMetadataTag(
            std::string_view str,
            size_t& pos,
            const Codebooks& codebooks,
            LocalIdParsingState expectedState,
            LocalIdParsingErrorBuilder* errorBuilder )
        {
            auto segment = extractSegment( str, pos );

            if( segment.empty() )
            {
                return std::nullopt;
            }

            // Find separator (- or ~)
            size_t sepPos = segment.find( '-' );

            if( sepPos == std::string_view::npos )
            {
                sepPos = segment.find( '~' );
            }

            if( sepPos == std::string_view::npos )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        expectedState, std::string{ "Invalid metadata tag format: '" } + std::string{ segment } + "'" );
                }

                skipSlash( pos );

                return std::nullopt;
            }

            auto prefix = segment.substr( 0, sepPos );
            auto value = segment.substr( sepPos + 1 );

            // Get codebook name from prefix
            CodebookName name;
            try
            {
                name = CodebookNames::fromPrefix( prefix );
            }
            catch( ... )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        expectedState, std::string{ "Unknown metadata tag prefix: '" } + std::string{ prefix } + "'" );
                }

                skipSlash( pos );

                return std::nullopt;
            }

            // Create tag using createTag() which handles isCustom automatically
            auto tag = codebooks[name].createTag( value );

            if( !tag.has_value() )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        expectedState,
                        std::string{ "Invalid metadata tag value for " } + std::string{ prefix } + ": '" +
                            std::string{ value } + "'" );
                }
            }

            skipSlash( pos );
            return tag;
        }
    } // namespace

    LocalId LocalIdBuilder::build() const
    {
        if( !isValid() )
        {
            throw std::invalid_argument{
                "Cannot build LocalId: VisVersion, primary item, and at least one metadata tag are required"
            };
        }

        return LocalId{ *this };
    }

    void LocalIdBuilder::toString( StringBuilder& builder ) const
    {
        // Format: /dnv-v2/vis-{version}/{primary}[/sec/{secondary}]/meta[/qty-{val}]...

        // Naming rule
        builder << "/dnv-v2";

        // VIS version
        if( m_visVersion.has_value() )
        {
            builder << "/vis-" << VisVersions::toString( *m_visVersion );
        }

        // Primary item
        if( m_primaryItem.has_value() )
        {
            builder << '/';
            m_primaryItem->toString( builder );
            builder << '/';
        }

        // Secondary item
        if( m_secondaryItem.has_value() )
        {
            builder << "sec/";
            m_secondaryItem->toString( builder );
            builder << '/';
        }

        // Verbose mode: Add common names
        if( m_verboseMode )
        {
            if( m_primaryItem.has_value() )
            {
                auto commonNamesVec = m_primaryItem->commonNames();
                for( const auto& [depth, name] : commonNamesVec )
                {
                    builder << '~';
                    appendNormalizedCommonName( name, builder );

                    // Append location if present
                    const auto& node = ( *m_primaryItem )[depth];
                    if( node.location().has_value() )
                    {
                        builder << '.' << node.location()->value();
                    }
                    builder << '/';
                }
            }

            if( m_secondaryItem.has_value() )
            {
                auto commonNamesVec = m_secondaryItem->commonNames();
                const char* prefix = "~for.";
                for( const auto& [depth, name] : commonNamesVec )
                {
                    builder << prefix;
                    if( std::string_view( prefix ) != "~" )
                    {
                        prefix = "~";
                    }

                    appendNormalizedCommonName( name, builder );

                    // Append location if present
                    const auto& node = ( *m_secondaryItem )[depth];
                    if( node.location().has_value() )
                    {
                        builder << '.' << node.location()->value();
                    }
                    builder << '/';
                }
            }
        }

        // Metadata section
        builder << "meta/";
        if( m_quantity.has_value() )
        {
            m_quantity->toString( builder );
        }
        if( m_content.has_value() )
        {
            m_content->toString( builder );
        }
        if( m_calculation.has_value() )
        {
            m_calculation->toString( builder );
        }
        if( m_state.has_value() )
        {
            m_state->toString( builder );
        }
        if( m_command.has_value() )
        {
            m_command->toString( builder );
        }
        if( m_type.has_value() )
        {
            m_type->toString( builder );
        }
        if( m_position.has_value() )
        {
            m_position->toString( builder );
        }
        if( m_detail.has_value() )
        {
            m_detail->toString( builder );
        }

        auto str = builder.toString();
        if( !str.empty() && str.back() == '/' )
        {
            builder.clear();
            builder << std::string_view( str.data(), str.size() - 1 );
        }
    }

    std::optional<LocalId> LocalIdBuilder::fromString( std::string_view localIdStr ) noexcept
    {
        return fromString( localIdStr, nullptr );
    }

    std::optional<LocalId> LocalIdBuilder::fromString( std::string_view localIdStr, ParsingErrors& errors ) noexcept
    {
        internal::LocalIdParsingErrorBuilder errorBuilder;
        auto result = fromString( localIdStr, &errorBuilder );
        errors = errorBuilder.build();

        return result;
    }

    std::optional<LocalId> LocalIdBuilder::fromString(
        std::string_view localIdStr, LocalIdParsingErrorBuilder* errorBuilder ) noexcept
    {
        if( localIdStr.empty() )
        {
            if( errorBuilder )
            {
                errorBuilder->addError( LocalIdParsingState::EmptyState, "LocalId string is empty" );
            }

            return std::nullopt;
        }

        if( localIdStr[0] != '/' )
        {
            if( errorBuilder )
            {
                errorBuilder->addError( LocalIdParsingState::Formatting, "LocalId must start with '/'" );
            }

            return std::nullopt;
        }

        size_t pos = 1; // Skip initial '/'

        // Parse naming rule
        if( !parseNamingRule( localIdStr, pos, errorBuilder ) )
        {
            return std::nullopt;
        }

        // Parse VIS version
        auto version = parseVisVersion( localIdStr, pos, errorBuilder );
        if( !version.has_value() )
        {
            return std::nullopt;
        }

        // Get Gmod and Codebooks for this version
        const auto& vis = VIS::instance();
        const auto& gmod = vis.gmod( *version );
        const auto& locations = vis.locations( *version );
        const auto& codebooks = vis.codebooks( *version );

        // Parse primary item
        auto primaryItem = parsePrimaryItem( localIdStr, pos, gmod, locations, errorBuilder );
        if( !primaryItem.has_value() )
        {
            return std::nullopt;
        }

        // Parse optional secondary item
        std::optional<GmodPath> secondaryItem;
        if( pos < localIdStr.size() )
        {
            auto peekSegment = extractSegment( localIdStr, pos );

            if( peekSegment == "sec" )
            {
                pos -= peekSegment.size(); // Rewind for parseSecondaryItem
                secondaryItem = parseSecondaryItem( localIdStr, pos, gmod, locations, errorBuilder );
            }
            else
            {
                pos -= peekSegment.size(); // Rewind
            }
        }

        // Skip verbose mode common names (segments starting with ~)
        bool hasVerboseMode = false;
        while( pos < localIdStr.size() )
        {
            auto segment = extractSegment( localIdStr, pos );
            if( segment.empty() || !nfx::string::startsWith( segment, '~' ) )
            {
                pos -= segment.size(); // Rewind
                break;
            }
            hasVerboseMode = true;
            skipSlash( pos );
        }

        // Parse /meta keyword
        if( pos >= localIdStr.size() )
        {
            if( errorBuilder )
            {
                errorBuilder->addError( LocalIdParsingState::Completeness, "Missing '/meta' section" );
            }

            return std::nullopt;
        }

        auto metaKeyword = extractSegment( localIdStr, pos );
        if( !nfx::string::equals( metaKeyword, "meta" ) )
        {
            if( errorBuilder )
            {
                errorBuilder->addError(
                    LocalIdParsingState::Completeness,
                    std::string{ "Expected '/meta', got '/" } + std::string{ metaKeyword } + "'" );
            }

            return std::nullopt;
        }

        skipSlash( pos );

        // Parse all metadata tags
        std::vector<MetadataTag> tags;
        while( pos < localIdStr.size() )
        {
            size_t peekPos = pos;
            auto segment = extractSegment( localIdStr, peekPos );

            if( segment.empty() )
            {
                break;
            }

            // Determine tag type from prefix
            size_t sepPos = segment.find( '-' );
            if( sepPos == std::string_view::npos )
            {
                sepPos = segment.find( '~' );
            }

            if( sepPos == std::string_view::npos )
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        LocalIdParsingState::Formatting,
                        std::string{ "Invalid metadata tag: '" } + std::string{ segment } + "'" );
                }

                pos = peekPos;
                if( pos < localIdStr.size() && localIdStr[pos] == '/' )
                {
                    skipSlash( pos );
                }

                continue;
            }

            auto prefix = segment.substr( 0, sepPos );
            LocalIdParsingState state;

            if( nfx::string::equals( prefix, "qty" ) )
            {
                state = LocalIdParsingState::MetaQuantity;
            }
            else if( nfx::string::equals( prefix, "cnt" ) )
            {
                state = LocalIdParsingState::MetaContent;
            }
            else if( nfx::string::equals( prefix, "calc" ) )
            {
                state = LocalIdParsingState::MetaCalculation;
            }
            else if( nfx::string::equals( prefix, "state" ) )
            {
                state = LocalIdParsingState::MetaState;
            }
            else if( nfx::string::equals( prefix, "cmd" ) )
            {
                state = LocalIdParsingState::MetaCommand;
            }
            else if( nfx::string::equals( prefix, "type" ) )
            {
                state = LocalIdParsingState::MetaType;
            }
            else if( nfx::string::equals( prefix, "pos" ) )
            {
                state = LocalIdParsingState::MetaPosition;
            }
            else if( nfx::string::equals( prefix, "detail" ) )
            {
                state = LocalIdParsingState::MetaDetail;
            }
            else
            {
                if( errorBuilder )
                {
                    errorBuilder->addError(
                        LocalIdParsingState::Formatting,
                        std::string{ "Unknown metadata tag prefix: '" } + std::string{ prefix } + "'" );
                }

                pos = peekPos;
                if( pos < localIdStr.size() && localIdStr[pos] == '/' )
                {
                    skipSlash( pos );
                }
                continue;
            }

            auto tag = parseMetadataTag( localIdStr, pos, codebooks, state, errorBuilder );
            if( tag.has_value() )
            {
                tags.emplace_back( std::move( *tag ) );
            }
        }

        if( tags.empty() )
        {
            if( errorBuilder )
            {
                errorBuilder->addError( LocalIdParsingState::Completeness, "At least one metadata tag is required" );
            }

            return std::nullopt;
        }

        // Build LocalId with all collected data
        try
        {
            // Start with version and primary item
            LocalIdBuilder builder = LocalIdBuilder::create( *version ).withPrimaryItem( *primaryItem );

            // Add optional secondary item
            if( secondaryItem.has_value() )
            {
                builder = builder.withSecondaryItem( *secondaryItem );
            }

            // Set verbose mode if we encountered ~ segments
            if( hasVerboseMode )
            {
                builder = builder.withVerboseMode( true );
            }

            // Add all metadata tags
            for( const auto& tag : tags )
            {
                builder = builder.withMetadataTag( tag );
            }
            return builder.build();
        }
        catch( const std::exception& e )
        {
            if( errorBuilder )
            {
                errorBuilder->addError( LocalIdParsingState::Completeness, e.what() );
            }

            return std::nullopt;
        }
    }
} // namespace dnv::vista::sdk
