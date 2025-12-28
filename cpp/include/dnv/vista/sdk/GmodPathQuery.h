/**
 * @file GmodPathQuery.h
 * @brief Query API for matching GmodPath instances
 * @details Provides fluent API for building queries to filter GmodPaths based on nodes and locations.
 *          Supports path-based queries (starting from existing path) and node-based queries (from scratch).
 */

#pragma once

#include "GmodPath.h"
#include "Locations.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dnv::vista::sdk
{
    class GmodPathQueryBuilder;

    /**
     * @class GmodPathQuery
     * @brief Immutable query for matching GmodPath instances
     * @details Created via GmodPathQueryBuilder. Provides match() method to test GmodPaths.
     *
     * @example
     * @code
     * auto basePath = GmodPath::fromString("411.1/C101.31-2", version).value();
     * auto query = GmodPathQueryBuilder::from(basePath)
     *     .withoutLocations()
     *     .build();
     *
     * bool matches = query.match(otherPath);
     * @endcode
     */
    class LocalIdQuery;
    class LocalIdQueryBuilder;

    class GmodPathQuery final
    {
        friend class GmodPathQueryBuilder;
        friend class LocalIdQuery;
        friend class LocalIdQueryBuilder;

    private:
        explicit GmodPathQuery( const GmodPathQueryBuilder* builder );

    public:
        /**
         * @brief Copy constructor
         * @param other The GmodPathQuery to copy from
         */
        GmodPathQuery( const GmodPathQuery& other );

        /** @brief Move constructor */
        GmodPathQuery( GmodPathQuery&& ) noexcept = default;

        /** @brief Destructor */
        ~GmodPathQuery() = default;

        /**
         * @brief Copy assignment
         * @param other The GmodPathQuery to copy from
         * @return Reference to this GmodPathQuery
         */
        GmodPathQuery& operator=( const GmodPathQuery& other );

        /**
         * @brief Move assignment
         * @return Reference to this GmodPathQuery
         */
        GmodPathQuery& operator=( GmodPathQuery&& ) noexcept = default;

        /**
         * @brief Match against a GmodPath
         * @param other The GmodPath to test
         * @return true if other matches the query criteria
         */
        [[nodiscard]] bool match( const GmodPath& other ) const;

    private:
        /**
         * @brief Get the builder that created this query
         * @return Pointer to builder
         */
        [[nodiscard]] const GmodPathQueryBuilder* builder() const noexcept;

        std::unique_ptr<GmodPathQueryBuilder> m_builder;
    };

    /**
     * @class GmodPathQueryBuilder
     * @brief Abstract base class for building GmodPath queries
     * @details Two concrete implementations:
     *          - Path: Start from existing GmodPath, modify specific nodes
     *          - Nodes: Build from scratch, specify individual nodes
     */
    class GmodPathQueryBuilder
    {
        friend class GmodPathQuery;

    private:
        /**
         * @struct NodeItem
         * @brief Internal filter item for node matching
         */
        struct NodeItem
        {
            GmodNode node;
            std::vector<Location> locations;
            bool matchAllLocations = false;
            bool ignoreInMatching = false;

            NodeItem() = delete;
            NodeItem( const GmodNode& n, const std::vector<Location>& locs );
        };

    public:
        class Path;
        class Nodes;

    private:
        /** @brief Default constructor */
        GmodPathQueryBuilder() = default;

    public:
        /** @brief Copy constructor */
        GmodPathQueryBuilder( const GmodPathQueryBuilder& ) = default;

        /** @brief Move constructor */
        GmodPathQueryBuilder( GmodPathQueryBuilder&& ) noexcept = default;

        /** @brief Virtual destructor */
        virtual ~GmodPathQueryBuilder() = default;

        /**
         * @brief Copy assignment
         * @return Reference to this GmodPathQueryBuilder
         */
        GmodPathQueryBuilder& operator=( const GmodPathQueryBuilder& ) = default;

        /**
         * @brief Move assignment
         * @return Reference to this GmodPathQueryBuilder
         */
        GmodPathQueryBuilder& operator=( GmodPathQueryBuilder&& ) noexcept = default;

        /**
         * @brief Create empty Nodes builder
         * @return New Nodes builder
         */
        [[nodiscard]] static Nodes empty();

        /**
         * @brief Create Path builder from existing GmodPath
         * @param path GmodPath to use as base
         * @return New Path builder
         */
        [[nodiscard]] static Path from( const GmodPath& path );

        /**
         * @brief Build immutable query
         * @return GmodPathQuery instance
         */
        [[nodiscard]] GmodPathQuery build() const;

        /**
         * @brief Internal match implementation
         * @param other GmodPath to test (nullptr returns false)
         * @return true if path matches the query criteria
         */
        [[nodiscard]] virtual bool match( const GmodPath* other ) const = 0;

    protected:
        /**
         * @brief Clone this builder (polymorphic copy)
         * @return Unique pointer to cloned builder
         */
        [[nodiscard]] virtual std::unique_ptr<GmodPathQueryBuilder> clone() const = 0;

        /**
         * @brief Common matching logic for Path and Nodes queries
         * @param filter Map of node codes to filter items
         * @param target The target GmodPath to match against (already converted to latest version)
         * @param checkIgnoreFlag Whether to check the ignoreInMatching flag
         * @return true if target matches all filter criteria
         */
        [[nodiscard]] static bool matchFilterAgainstTarget(
            const std::unordered_map<std::string, NodeItem>& filter, const GmodPath& target, bool checkIgnoreFlag );

        /** @brief Map of node codes to filter items for matching */
        std::unordered_map<std::string, NodeItem> m_filter;
    };

    /**
     * @class GmodPathQueryBuilder::Path
     * @brief Path-based query builder
     */
    class GmodPathQueryBuilder::Path final : public GmodPathQueryBuilder
    {
    public:
        /** @brief Default constructor */
        Path() = delete;

        /**
         * @brief Construct from GmodPath
         * @param path The base GmodPath to use for this query
         */
        explicit Path( const GmodPath& path );

        /** @brief Copy constructor */
        Path( const Path& ) = default;

        /** @brief Move constructor */
        Path( Path&& ) noexcept = default;

        /** @brief Destructor */
        virtual ~Path() override = default;

        /**
         * @brief Copy assignment
         * @return Reference to this Path builder
         */
        Path& operator=( const Path& ) = default;

        /**
         * @brief Move assignment
         * @return Reference to this Path builder
         */
        Path& operator=( Path&& ) noexcept = default;

        /**
         * @brief Get the base GmodPath
         * @return Reference to the GmodPath
         */
        [[nodiscard]] const GmodPath& path() const noexcept;

        /**
         * @brief Configure node to match all locations
         * @param select Function to select node from set nodes dictionary
         * @param matchAllLocations If true, match any location individualization
         * @return New Path builder with node configured
         */
        [[nodiscard]] Path withNode(
            std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
            bool matchAllLocations = false ) const&;

        /**
         * @brief Configure node to match all locations
         * @param select Function to select node from set nodes dictionary
         * @param matchAllLocations If true, match any location individualization
         * @return New Path builder with node configured
         */
        [[nodiscard]] Path withNode(
            std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
            bool matchAllLocations = false ) &&;

        /**
         * @brief Configure node with specific locations
         * @param select Function to select node from set nodes dictionary
         * @param locations Locations to match (empty = no locations)
         * @return New Path builder with node configured
         */
        [[nodiscard]] Path withNode(
            std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
            const std::vector<Location>& locations ) const&;

        /**
         * @brief Configure node with specific locations
         * @param select Function to select node from set nodes dictionary
         * @param locations Locations to match (empty = no locations)
         * @return New Path builder with node configured
         */
        [[nodiscard]] Path withNode(
            std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
            const std::vector<Location>& locations ) &&;

        /**
         * @brief Ignore nodes before specified node in path
         * @param select Function to select node from full path nodes
         * @return New Path builder with nodes ignored
         * @throws std::invalid_argument if node not in path
         */
        [[nodiscard]] Path withAnyNodeBefore(
            std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select ) const&;

        /**
         * @brief Ignore nodes before specified node in path
         * @param select Function to select node from full path nodes
         * @return New Path builder with nodes ignored
         * @throws std::invalid_argument if node not in path
         */
        [[nodiscard]] Path withAnyNodeBefore(
            std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select ) &&;

        /**
         * @brief Ignore all location individualizations
         * @return New Path builder with locations ignored
         */
        [[nodiscard]] Path withoutLocations() const&;

        /**
         * @brief Ignore all location individualizations
         * @return New Path builder with locations ignored
         */
        [[nodiscard]] Path withoutLocations() &&;

        /**
         * @brief Internal match implementation
         * @param other GmodPath to test (nullptr returns false)
         * @return true if path matches the query criteria
         */
        [[nodiscard]] virtual bool match( const GmodPath* other ) const override;

    private:
        /** @brief Clone this builder */
        [[nodiscard]] virtual std::unique_ptr<GmodPathQueryBuilder> clone() const override;

        /** @brief The base GmodPath used for this query */
        GmodPath m_gmodPath;

        /** @brief Map of set nodes (nodes that can be individually configured) */
        std::unordered_map<std::string, const GmodNode*> m_setNodes;

        /** @brief Map of all nodes in the path */
        std::unordered_map<std::string, const GmodNode*> m_nodes;
    };

    /**
     * @class GmodPathQueryBuilder::Nodes
     * @brief Node-based query builder (build from scratch)
     */
    class GmodPathQueryBuilder::Nodes final : public GmodPathQueryBuilder
    {
        friend class GmodPathQueryBuilder;

    private:
        /** @brief Default constructor */
        Nodes() = default;

    public:
        /** @brief Copy constructor */
        Nodes( const Nodes& ) = default;

        /** @brief Move constructor */
        Nodes( Nodes&& ) noexcept = default;

        /** @brief Destructor */
        virtual ~Nodes() override = default;

        /**
         * @brief Copy assignment
         * @return Reference to this Nodes builder
         */
        Nodes& operator=( const Nodes& ) = default;

        /**
         * @brief Move assignment
         * @return Reference to this Nodes builder
         */
        Nodes& operator=( Nodes&& ) noexcept = default;

        /**
         * @brief Add node to match with location settings
         * @param node Node to match
         * @param matchAllLocations If true, match any location individualization
         * @return New Nodes builder with node added
         */
        [[nodiscard]] Nodes withNode( const GmodNode& node, bool matchAllLocations = false ) const&;

        /**
         * @brief Add node to match with location settings
         * @param node Node to match
         * @param matchAllLocations If true, match any location individualization
         * @return New Nodes builder with node added
         */
        [[nodiscard]] Nodes withNode( const GmodNode& node, bool matchAllLocations = false ) &&;

        /**
         * @brief Add node to match with specific locations
         * @param node Node to match
         * @param locations Locations to match (empty = no locations)
         * @return New Nodes builder with node added
         */
        [[nodiscard]] Nodes withNode( const GmodNode& node, const std::vector<Location>& locations ) const&;

        /**
         * @brief Add node to match with specific locations
         * @param node Node to match
         * @param locations Locations to match (empty = no locations)
         * @return New Nodes builder with node added
         */
        [[nodiscard]] Nodes withNode( const GmodNode& node, const std::vector<Location>& locations ) &&;

        /**
         * @brief Internal match implementation
         * @param other GmodPath to test (nullptr returns false)
         * @return true if path matches the query criteria
         */
        [[nodiscard]] virtual bool match( const GmodPath* other ) const override;

    private:
        /** @brief Clone this builder */
        [[nodiscard]] virtual std::unique_ptr<GmodPathQueryBuilder> clone() const override;
    };
} // namespace dnv::vista::sdk
