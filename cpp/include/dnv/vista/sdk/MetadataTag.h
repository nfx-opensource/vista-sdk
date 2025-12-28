/**
 * @file MetadataTag.h
 * @brief Metadata tag for VIS Local ID content/quantity/state markers
 * @details Represents a metadata tag in a VIS Local ID with a codebook name,
 *          value, and custom flag. Tags appear as "prefix-value" or "prefix~value"
 *          in the Local ID string format.
 */

#pragma once

#include "CodebookName.h"
#include "StringBuilder.h"

namespace dnv::vista::sdk
{
    /**
     * @brief Represents a metadata tag with a codebook name and value
     * @details A metadata tag combines a CodebookName (e.g., Quantity, Content) with
     *          a string value (e.g., "temperature", "oil"). Custom tags are marked
     *          with '~' separator, standard tags use '-' separator.
     *          MetadataTag instances can only be created by the Codebook.
     */
    class MetadataTag final
    {
        friend class Codebook;
        friend class MetadataTagsQueryBuilder;

    private:
        /**
         * @brief Internal constructor - Constructs a MetadataTag object
         * @param name The codebook this tag belongs to
         * @param value The value of the metadata tag
         * @param isCustom Indicates whether the tag is custom (default is false)
         */
        inline explicit MetadataTag( CodebookName name, std::string value, bool isCustom = false ) noexcept;

    public:
        /** @brief Default constructor */
        MetadataTag() = delete;

        /** @brief Copy constructor */
        MetadataTag( const MetadataTag& ) = default;

        /** @brief Move constructor */
        MetadataTag( MetadataTag&& ) noexcept = default;

        /** @brief Destructor */
        ~MetadataTag() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this object
         */
        MetadataTag& operator=( const MetadataTag& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this object
         */
        MetadataTag& operator=( MetadataTag&& ) noexcept = default;

        /**
         * @brief Equality comparison
         * @param other The other MetadataTag to compare with
         * @return true if same CodebookName and value
         */
        [[nodiscard]] bool operator==( const MetadataTag& other ) const noexcept;

        /**
         * @brief Inequality comparison
         * @param other The other MetadataTag to compare with
         * @return true if different values
         */
        inline bool operator!=( const MetadataTag& other ) const noexcept;

        /**
         * @brief Get the codebook name
         * @return The CodebookName enum value
         */
        [[nodiscard]] inline CodebookName name() const noexcept;

        /**
         * @brief Get the tag value
         * @return The string value
         */
        [[nodiscard]] inline const std::string& value() const noexcept;

        /**
         * @brief Get the prefix separator character
         * @return '~' for custom tags, '-' for standard tags
         */
        [[nodiscard]] inline char prefix() const noexcept;

        /**
         * @brief Check if this is a custom tag
         * @return true if custom, false if standard
         */
        [[nodiscard]] inline bool isCustom() const noexcept;

        /**
         * @brief Appends a fully formatted string representation of the metadata tag to a string builder
         * @param builder The StringBuilder to write to
         * @param separator The separator character to append after the value
         * @details Formats the tag as "prefix-value/" or "prefix~value/" directly into the StringBuilder
         */
        inline void toString( StringBuilder& builder, char separator = '/' ) const;

    private:
        /** @brief The name of the metadata tag, represented by a CodebookName enum value */
        CodebookName m_name;

        /** @brief A boolean flag indicating whether this is a custom tag (true) or a standard tag (false) */
        bool m_custom;

        /** @brief The string value associated with the metadata tag */
        std::string m_value;
    };
} // namespace dnv::vista::sdk

#include "detail/MetadataTag.inl"
