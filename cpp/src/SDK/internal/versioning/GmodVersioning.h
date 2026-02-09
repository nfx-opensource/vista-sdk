/**
 * @file GmodVersioning.h
 * @brief Cross-version conversion system for Gmod nodes, paths, and LocalIds
 * @details Provides functionality to convert Gmod objects between different VIS versions.
 *          Uses versioning metadata to handle code changes, merges, and assignment changes.
 */

#pragma once

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/VisVersions.h"
#include "SDK/dto/GmodVersioningDto.h"

namespace dnv::vista::sdk::internal
{
    /**
     * @brief Type of conversion operation for Gmod nodes between versions
     */
    enum class ConversionType : std::uint8_t
    {
        None = 0,                  ///< No operations
        ChangeCode = 1 << 0,       ///< Node code changed
        Merge = 1 << 1,            ///< Node merged into another
        Move = 1 << 2,             ///< Node moved to different location
        AssignmentChange = 1 << 3, ///< Normal assignment changed
        AssignmentDelete = 1 << 4  ///< Normal assignment deleted
    };

    /**
     * @brief Represents a conversion operation for a specific Gmod node
     */
    struct GmodNodeConversion
    {
        std::uint8_t operations = 0;              ///< Bitflags of operations to perform (ConversionType flags)
        std::string source;                       ///< Source node code
        std::optional<std::string> target;        ///< Target node code (if applicable)
        std::optional<std::string> oldAssignment; ///< Old normal assignment code
        std::optional<std::string> newAssignment; ///< New normal assignment code
        std::optional<bool> deleteAssignment;     ///< Whether assignment was deleted
    };

    /**
     * @brief Versioning information for a specific VIS version
     */
    struct GmodVersioningNode
    {
        VisVersion visVersion;                                          ///< The VIS version this node represents
        HashMap<std::string, GmodNodeConversion> versioningNodeChanges; ///< Map of code changes

        /**
         * @brief Constructor from DTO
         * @param version VIS version for this node
         * @param dto Map of conversion DTOs indexed by node code
         */
        explicit GmodVersioningNode(
            VisVersion version, const std::unordered_map<std::string, GmodNodeConversionDto>& dto );

        /**
         * @brief Get code changes for a specific node code
         * @param code Node code to look up
         * @return Optional pointer to conversion info if found, std::nullopt otherwise
         */
        [[nodiscard]] std::optional<const GmodNodeConversion*> codeChanges( std::string_view code ) const noexcept;
    };

    /**
     * @brief Class responsible for converting Gmod objects to a higher version
     */
    class GmodVersioning final
    {
    public:
        /**
         * @brief Parse ConversionType from string
         * @throws std::invalid_argument If type string is invalid
         */
        static ConversionType parseConversionType( const std::string& type );

        /**
         * @brief Constructor
         *
         * @param dto Dictionary of GmodVersioningDto objects indexed by version string
         */
        explicit GmodVersioning( const std::unordered_map<std::string, GmodVersioningDto>& dto );

        /** @brief Default constructor. */
        GmodVersioning() = delete;

        /** @brief Copy constructor */
        GmodVersioning( const GmodVersioning& ) = delete;

        /** @brief Move constructor */
        GmodVersioning( GmodVersioning&& other ) noexcept;

        /** @brief Destructor */
        ~GmodVersioning() = default;

        /** @brief Copy assignment operator */
        GmodVersioning& operator=( const GmodVersioning& ) = delete;

        /** @brief Move assignment operator */
        GmodVersioning& operator=( GmodVersioning&& ) noexcept = default;

        /**
         * @brief Convert a GmodNode from one version to a higher version
         * @param sourceVersion VIS version of the source node
         * @param sourceNode Node to convert
         * @param targetVersion Target VIS version (must be higher than source)
         * @return Converted node if successful, std::nullopt otherwise
         * @throws std::invalid_argument If targetVersion is not higher than sourceVersion
         */
        [[nodiscard]] std::optional<GmodNode> convertNode(
            VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const;

        /**
         * @brief Convert a GmodNode with cached target Gmod
         * @param sourceVersion VIS version of the source node
         * @param sourceNode Node to convert
         * @param targetVersion Target VIS version (must be higher than source)
         * @param targetGmod Cached target Gmod instance for performance
         * @return Converted node if successful, std::nullopt otherwise
         * @throws std::invalid_argument If targetVersion is not higher than sourceVersion
         */
        [[nodiscard]] std::optional<GmodNode> convertNode(
            VisVersion sourceVersion,
            const GmodNode& sourceNode,
            VisVersion targetVersion,
            const Gmod& targetGmod ) const;

        /**
         * @brief Convert a GmodPath from one VIS version to another
         * @param sourceVersion VIS version of the source path
         * @param sourcePath Path to convert
         * @param targetVersion Target VIS version (must be higher than source)
         * @return Converted path if successful, std::nullopt otherwise
         * @throws std::invalid_argument If source or target versions are invalid or source >= target
         */
        [[nodiscard]] std::optional<GmodPath> convertPath(
            VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const;

        /**
         * @brief Convert a LocalIdBuilder from one version to a higher version
         * @param sourceLocalId LocalIdBuilder to convert
         * @param targetVersion Target VIS version (must be higher than source)
         * @return Converted LocalIdBuilder if successful, std::nullopt otherwise
         * @throws std::invalid_argument If sourceLocalId has no version set
         */
        [[nodiscard]] std::optional<LocalIdBuilder> convertLocalId(
            const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const;

        /**
         * @brief Convert a LocalId from one version to a higher version
         * @param sourceLocalId LocalId to convert
         * @param targetVersion Target VIS version (must be higher than source)
         * @return Converted LocalId if successful, std::nullopt otherwise
         */
        [[nodiscard]] std::optional<LocalId> convertLocalId(
            const LocalId& sourceLocalId, VisVersion targetVersion ) const;

    private:
        /**
         * @brief Add a node to the path, handling parent chain reconstruction
         * @param gmod Target Gmod instance
         * @param path Path being constructed
         * @param node Node to add to the path
         */
        void addToPath( const Gmod& gmod, StackVector<GmodNode, 16>& path, const GmodNode& node ) const;

        /**
         * @brief Convert a single node from one version to the next version (internal helper)
         * @param sourceVersion VIS version of the source node
         * @param sourceNode Node to convert
         * @param targetVersion Target VIS version (must be exactly one version higher)
         * @return Converted node if successful, std::nullopt otherwise
         * @throws std::invalid_argument If targetVersion is not exactly one version higher than sourceVersion
         */
        [[nodiscard]] std::optional<GmodNode> convertNodeInternal(
            VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const;

        /**
         * @brief Convert a single node with cached Gmod (internal helper)
         * @param sourceVersion VIS version of the source node
         * @param sourceNode Node to convert
         * @param targetVersion Target VIS version (must be exactly one version higher)
         * @param targetGmod Cached target Gmod instance for performance
         * @return Converted node if successful, std::nullopt otherwise
         */
        [[nodiscard]] std::optional<GmodNode> convertNodeInternal(
            VisVersion sourceVersion,
            const GmodNode& sourceNode,
            VisVersion targetVersion,
            const Gmod& targetGmod ) const;

        /**
         * @brief Get versioning node for a specific version
         * @param visVersion Version to look up
         * @return Optional pointer to versioning node if found, std::nullopt otherwise
         */
        [[nodiscard]] std::optional<const GmodVersioningNode*> versioningNode( VisVersion visVersion ) const noexcept;

        /**
         * @brief Validate that source version is less than target version
         * @throws std::invalid_argument If versions are invalid or source >= target
         */
        void validateSourceAndTargetVersions( VisVersion sourceVersion, VisVersion targetVersion ) const;

        /**
         * @brief Validate that target version is exactly one version higher than source
         * @throws std::invalid_argument If versions don't satisfy the requirement
         */
        void validateSourceAndTargetVersionPair( VisVersion sourceVersion, VisVersion targetVersion ) const;

        /** @brief Hash map storing versioning nodes indexed by VIS version */
        std::unordered_map<VisVersion, GmodVersioningNode> m_versioningsMap;
    };
} // namespace dnv::vista::sdk::internal
