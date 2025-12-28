/**
 * @file MetadataTagsQuery.h
 * @brief Query API for matching LocalId metadata tags
 * @details Provides fluent API for building queries to filter LocalIds based on their metadata tags.
 *          Supports exact matching (must have only these tags) or subset matching (must have at least these tags).
 */

#pragma once

#include "CodebookName.h"
#include "MetadataTag.h"

#include <unordered_map>

namespace dnv::vista::sdk
{
    class LocalId;
    class MetadataTagsQueryBuilder;

    /**
     * @class MetadataTagsQuery
     * @brief Immutable query for matching LocalId metadata tags
     * @details Created via MetadataTagsQueryBuilder. Provides match() method to test LocalIds.
     *
     * @par Example:
     * @code
     * auto query = MetadataTagsQueryBuilder::empty()
     *     .withTag(CodebookName::Quantity, "temperature")
     *     .withTag(CodebookName::Content, "water")
     *     .withAllowOtherTags(false)  // Exact match
     *     .build();
     *
     * bool matches = query.match(localId);
     * @endcode
     */
    class MetadataTagsQuery final
    {
        friend class MetadataTagsQueryBuilder;

    private:
        explicit MetadataTagsQuery( const MetadataTagsQueryBuilder* builder );

    public:
        /** @brief Copy constructor */
        MetadataTagsQuery( const MetadataTagsQuery& ) = default;

        /** @brief Move constructor */
        MetadataTagsQuery( MetadataTagsQuery&& ) noexcept = default;

        /** @brief Destructor */
        ~MetadataTagsQuery() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this MetadataTagsQuery
         */
        MetadataTagsQuery& operator=( const MetadataTagsQuery& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this MetadataTagsQuery
         */
        MetadataTagsQuery& operator=( MetadataTagsQuery&& ) noexcept = default;

        /**

         * @param localId LocalId to test
         * @return true if localId matches the query criteria
         */
        [[nodiscard]] bool match( const LocalId& localId ) const;

    private:
        std::unordered_map<CodebookName, MetadataTag> m_tags;
        bool m_matchExact = false;
    };

    /**
     * @class MetadataTagsQueryBuilder
     * @brief Fluent builder for MetadataTagsQuery
     * @details Allows specifying metadata tags to match and whether to allow additional tags.
     *          Two matching modes:
     *          - Subset match (default): LocalId must have AT LEAST the specified tags (can have more)
     *          - Exact match: LocalId must have EXACTLY the specified tags (no more, no less)
     *
     * @par Example:
     * @code
     * // Subset matching (allows extra tags)
     * auto query1 = MetadataTagsQueryBuilder::empty()
     *     .withTag(CodebookName::Quantity, "temperature")
     *     .build();
     *
     * // Exact matching (no extra tags allowed)
     * auto query2 = MetadataTagsQueryBuilder::from(localId, false)
     *     .withAllowOtherTags(false)
     *     .build();
     * @endcode
     */
    class MetadataTagsQueryBuilder final
    {
        friend class MetadataTagsQuery;

    private:
        /** @brief Default constructor */
        MetadataTagsQueryBuilder() = default;

    public:
        /** @brief Copy constructor */
        MetadataTagsQueryBuilder( const MetadataTagsQueryBuilder& ) = default;

        /** @brief Move constructor */
        MetadataTagsQueryBuilder( MetadataTagsQueryBuilder&& ) noexcept = default;

        /** @brief Destructor */
        ~MetadataTagsQueryBuilder() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this MetadataTagsQueryBuilder
         */
        MetadataTagsQueryBuilder& operator=( const MetadataTagsQueryBuilder& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this MetadataTagsQueryBuilder
         */
        MetadataTagsQueryBuilder& operator=( MetadataTagsQueryBuilder&& ) noexcept = default;

        /**
         * @brief Create empty builder (matches any LocalId by default)
         * @return New empty builder
         */
        [[nodiscard]] static MetadataTagsQueryBuilder empty();

        /**
         * @brief Create builder from existing LocalId
         * @param localId LocalId to copy tags from
         * @param allowOtherTags If true, allow additional tags; if false, exact match only
         * @return Builder initialized with localId's tags
         */
        [[nodiscard]] static MetadataTagsQueryBuilder from( const LocalId& localId, bool allowOtherTags = true );

        /**
         * @brief Add a metadata tag to match
         * @param name Codebook name
         * @param value Tag value
         * @return New builder with tag added
         * @note If a tag for this codebook already exists, it will be replaced
         */
        [[nodiscard]] MetadataTagsQueryBuilder withTag( CodebookName name, const std::string& value ) const&;

        /**
         * @brief Add a metadata tag to match
         * @param name Codebook name
         * @param value Tag value
         * @return New builder with tag added
         * @note If a tag for this codebook already exists, it will be replaced
         */
        [[nodiscard]] inline MetadataTagsQueryBuilder withTag( CodebookName name, const std::string& value ) &&;

        /**
         * @brief Add a metadata tag to match
         * @param tag MetadataTag to match
         * @return New builder with tag added
         * @note If a tag for this codebook already exists, it will be replaced
         */
        [[nodiscard]] inline MetadataTagsQueryBuilder withTag( const MetadataTag& tag ) const&;

        /**
         * @brief Add a metadata tag to match
         * @param tag MetadataTag to match
         * @return New builder with tag added
         * @note If a tag for this codebook already exists, it will be replaced
         */
        [[nodiscard]] inline MetadataTagsQueryBuilder withTag( const MetadataTag& tag ) &&;

        /**
         * @brief Set whether to allow tags not specified in the query
         * @param allowOthers If true (default), LocalId can have extra tags (subset matching)
         *                    If false, LocalId must have EXACTLY the specified tags (exact matching)
         * @return New builder with setting updated
         */
        [[nodiscard]] inline MetadataTagsQueryBuilder withAllowOtherTags( bool allowOthers ) const&;

        /**
         * @brief Set whether to allow tags not specified in the query
         * @param allowOthers If true (default), LocalId can have extra tags (subset matching)
         *                    If false, LocalId must have EXACTLY the specified tags (exact matching)
         * @return New builder with setting updated
         */
        [[nodiscard]] inline MetadataTagsQueryBuilder withAllowOtherTags( bool allowOthers ) &&;

        /**
         * @brief Build immutable query
         * @return MetadataTagsQuery instance
         */
        [[nodiscard]] inline MetadataTagsQuery build() const;

    private:
        std::unordered_map<CodebookName, MetadataTag> m_tags;
        bool m_matchExact = false;
    };
} // namespace dnv::vista::sdk

#include "detail/MetadataTagsQuery.inl"
