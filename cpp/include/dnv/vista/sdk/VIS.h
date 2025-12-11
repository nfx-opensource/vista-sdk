/**
 * @file VIS.h
 * @brief VIS singleton registry for accessing Gmod, Codebooks, and Locations
 * @details Central entry point for Vista SDK - provides cached access to versioned resources
 */

#pragma once

#include "VisVersions.h"

#include <vector>

namespace dnv::vista::sdk
{
    /**
     * @brief VIS singleton registry providing access to Vista SDK versioned resources
     * @details Central entry point for Vista SDK. Provides version management and cached
     *          access to Gmod, Codebooks, and Locations for all VIS versions.
     *          Uses Meyer's singleton pattern for thread-safe lazy initialization.
     */
    class VIS final
    {
    private:
        /** @brief Default constructor */
        VIS() = default;

        /** @brief Copy constructor */
        VIS( const VIS& ) = delete;

        /** @brief Move constructor */
        VIS( VIS&& ) noexcept = delete;

    public:
        /** @brief Destructor */
        ~VIS() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this object
         */
        VIS& operator=( const VIS& ) = delete;

        /**
         * @brief Move assignment operator
         * @return Reference to this object
         */
        VIS& operator=( VIS&& ) noexcept = delete;

        /**
         * @brief Get the singleton instance
         * @return Reference to the singleton VIS instance
         */
        static const VIS& instance();

        /**
         * @brief Get all available VIS versions
         * @return Vector containing all supported VIS versions in lexicographical order
         */
        const std::vector<VisVersion>& versions() const noexcept;

        /**
         * @brief Get the latest VIS version
         * @return The most recent VIS version
         */
        VisVersion latest() const noexcept;
    };
} // namespace dnv::vista::sdk
