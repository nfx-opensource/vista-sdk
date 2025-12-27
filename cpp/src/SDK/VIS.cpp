/**
 * @file VIS.cpp
 * @brief Implementation of VIS singleton registry
 * @details Provides version management and will provide cached access to Gmod, Codebooks, and Locations
 */

#include "dnv/vista/sdk/VIS.h"

#include "dnv/vista/sdk/detail/Containers.h"
#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/Locations.h"
#include "dto/CodebooksDto.h"
#include "dto/GmodDto.h"
#include "dto/LocationsDto.h"
#include "internal/versioning/GmodVersioning.h"
#include "VisVersionsExtensions.h"

#include <EmbeddedResources/EmbeddedResources.h>

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_set>

namespace dnv::vista::sdk
{
    namespace
    {
        auto& gmodCache()
        {
            static HashMap<VisVersion, Gmod> cache;
            return cache;
        }

        auto& gmodMutex()
        {
            static std::shared_mutex mutex;
            return mutex;
        }

        auto& codebooksCache()
        {
            static HashMap<VisVersion, Codebooks> cache;
            return cache;
        }

        auto& codebooksMutex()
        {
            static std::shared_mutex mutex;
            return mutex;
        }

        auto& locationsCache()
        {
            static HashMap<VisVersion, Locations> cache;
            return cache;
        }

        auto& locationsMutex()
        {
            static std::shared_mutex mutex;
            return mutex;
        }

        auto& gmodVersioningCache()
        {
            static std::unique_ptr<internal::GmodVersioning> cache;
            return cache;
        }

        auto& gmodVersioningMutex()
        {
            static std::mutex mutex;
            return mutex;
        }
    } // namespace

    namespace internal
    {
        static const GmodVersioning& gmodVersioning()
        {
            if( gmodVersioningCache() )
            {
                return *gmodVersioningCache();
            }

            std::lock_guard lock( gmodVersioningMutex() );

            if( gmodVersioningCache() )
            {
                return *gmodVersioningCache();
            }

            auto dto = EmbeddedResources::gmodVersioning();

            if( !dto.has_value() )
            {
                throw std::invalid_argument{ "Invalid state" };
            }

            gmodVersioningCache() = std::make_unique<GmodVersioning>( *dto );
            return *gmodVersioningCache();
        }
    } // namespace internal

    const VIS& VIS::instance()
    {
        static VIS instance;

        return instance;
    }

    VisVersion VIS::latest() const noexcept
    {
        return VisVersions::latest();
    }

    const std::vector<VisVersion>& VIS::versions() const noexcept
    {
        static const std::vector<VisVersion> versions = []() {
            const auto arr = VisVersions::all();
            return std::vector<VisVersion>( arr.begin(), arr.end() );
        }();

        return versions;
    }

    const Gmod& VIS::gmod( VisVersion visVersion ) const
    {
        // Fast path: read-only access
        {
            std::shared_lock lock( gmodMutex() );
            if( auto* cached = gmodCache().find( visVersion ) )
            {
                return *cached;
            }
        }

        // Slow path: load and cache
        std::unique_lock lock( gmodMutex() );

        // Double-check (another thread might have loaded it)
        if( auto* cached = gmodCache().find( visVersion ) )
        {
            return *cached;
        }

        // Load from embedded resource
        auto versionStr = VisVersions::toString( visVersion );
        auto dto = EmbeddedResources::gmod( versionStr );

        if( !dto.has_value() )
        {
            throw std::out_of_range{ "Gmod not available for version: " + std::string{ versionStr } };
        }

        gmodCache().emplace( visVersion, Gmod{ visVersion, *dto } );
        return *gmodCache().find( visVersion );
    }

    const Codebooks& VIS::codebooks( VisVersion visVersion ) const
    {
        // Fast path: read-only access
        {
            std::shared_lock lock( codebooksMutex() );
            if( auto* cached = codebooksCache().find( visVersion ) )
            {
                return *cached;
            }
        }

        // Slow path: load and cache
        std::unique_lock lock( codebooksMutex() );

        // Double-check
        if( auto* cached = codebooksCache().find( visVersion ) )
        {
            return *cached;
        }

        // Load from embedded resource
        auto versionStr = VisVersions::toString( visVersion );
        auto dto = EmbeddedResources::codebooks( versionStr );

        if( !dto.has_value() )
        {
            throw std::out_of_range{ "Codebooks not available for version: " + std::string{ versionStr } };
        }

        codebooksCache().emplace( visVersion, Codebooks{ visVersion, *dto } );
        return *codebooksCache().find( visVersion );
    }

    const Locations& VIS::locations( VisVersion visVersion ) const
    {
        // Fast path: read-only access
        {
            std::shared_lock lock( locationsMutex() );
            if( auto* cached = locationsCache().find( visVersion ) )
            {
                return *cached;
            }
        }

        // Slow path: load and cache
        std::unique_lock lock( locationsMutex() );

        // Double-check
        if( auto* cached = locationsCache().find( visVersion ) )
        {
            return *cached;
        }

        // Load from embedded resource
        auto versionStr = VisVersions::toString( visVersion );
        auto dto = EmbeddedResources::locations( versionStr );

        if( !dto.has_value() )
        {
            throw std::out_of_range{ "Locations not available for version: " + std::string{ versionStr } };
        }

        locationsCache().emplace( visVersion, Locations{ visVersion, *dto } );
        return *locationsCache().find( visVersion );
    }

    std::optional<GmodNode> VIS::convertNode(
        VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const
    {
        return internal::gmodVersioning().convertNode( sourceVersion, sourceNode, targetVersion );
    }

    std::optional<GmodNode> VIS::convertNode(
        const GmodNode& sourceNode, VisVersion targetVersion, [[maybe_unused]] const GmodNode* sourceParent ) const
    {
        return convertNode( sourceNode.version(), sourceNode, targetVersion );
    }

    std::optional<GmodPath> VIS::convertPath(
        VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const
    {
        return internal::gmodVersioning().convertPath( sourceVersion, sourcePath, targetVersion );
    }

    std::optional<GmodPath> VIS::convertPath( const GmodPath& sourcePath, VisVersion targetVersion ) const
    {
        return convertPath( sourcePath.version(), sourcePath, targetVersion );
    }

    std::optional<LocalIdBuilder> VIS::convertLocalId(
        const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const
    {
        return internal::gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
    }

    std::optional<LocalId> VIS::convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion ) const
    {
        return internal::gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
    }

    std::unordered_map<VisVersion, const Gmod&> VIS::gmodsMap( const std::vector<VisVersion>& visVersions ) const
    {
        for( const auto& version : visVersions )
        {
            auto versionStr = VisVersions::toString( version );
            if( versionStr.empty() )
            {
                throw std::invalid_argument{ "Invalid VIS version provided" };
            }
        }

        std::unordered_set<VisVersion> uniqueVersions( visVersions.begin(), visVersions.end() );

        std::unordered_map<VisVersion, const Gmod&> result;
        result.reserve( uniqueVersions.size() );

        for( const auto& version : uniqueVersions )
        {
            result.emplace( version, gmod( version ) );
        }

        return result;
    }

    std::unordered_map<VisVersion, const Codebooks&> VIS::codebooksMap(
        const std::vector<VisVersion>& visVersions ) const
    {
        for( const auto& version : visVersions )
        {
            auto versionStr = VisVersions::toString( version );
            if( versionStr.empty() )
            {
                throw std::invalid_argument{ "Invalid VIS version provided" };
            }
        }

        std::unordered_set<VisVersion> uniqueVersions( visVersions.begin(), visVersions.end() );

        std::unordered_map<VisVersion, const Codebooks&> result;
        result.reserve( uniqueVersions.size() );

        for( const auto& version : uniqueVersions )
        {
            result.emplace( version, codebooks( version ) );
        }

        return result;
    }

    std::unordered_map<VisVersion, const Locations&> VIS::locationsMap(
        const std::vector<VisVersion>& visVersions ) const
    {
        for( const auto& version : visVersions )
        {
            auto versionStr = VisVersions::toString( version );
            if( versionStr.empty() )
            {
                throw std::invalid_argument{ "Invalid VIS version provided" };
            }
        }

        std::unordered_set<VisVersion> uniqueVersions( visVersions.begin(), visVersions.end() );

        std::unordered_map<VisVersion, const Locations&> result;
        result.reserve( uniqueVersions.size() );

        for( const auto& version : uniqueVersions )
        {
            result.emplace( version, locations( version ) );
        }

        return result;
    }
} // namespace dnv::vista::sdk
