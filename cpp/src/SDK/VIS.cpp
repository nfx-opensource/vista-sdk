/**
 * @file VIS.cpp
 * @brief Implementation of VIS singleton registry
 * @details Provides version management and will provide cached access to Gmod, Codebooks, and Locations
 */

#include "dnv/vista/sdk/VIS.h"

#include "VisVersionsExtensions.h"

namespace dnv::vista::sdk
{
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
} // namespace dnv::vista::sdk
