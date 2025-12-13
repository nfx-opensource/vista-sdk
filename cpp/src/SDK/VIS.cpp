/**
 * @file VIS.cpp
 * @brief Implementation of VIS singleton registry
 * @details Provides version management and will provide cached access to Gmod, Codebooks, and Locations
 */

#include "dnv/vista/sdk/VIS.h"

#include "dnv/vista/sdk/detail/Containers.h"
#include "dnv/vista/sdk/Codebooks.h"
#include "dto/CodebooksDto.h"
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
    } // namespace

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
} // namespace dnv::vista::sdk
