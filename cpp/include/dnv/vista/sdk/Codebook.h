/**
 * @file Codebook.h
 * @brief VIS codebook representation with validation and metadata tag creation
 * @details Represents a VIS codebook containing standard values, groups, and validation logic.
 *          Codebooks are used to validate metadata tag values and create MetadataTag instances
 *          for use in VIS Local IDs. Special validation logic is provided for Position codebook.
 */

#pragma once

#include "detail/Containers.h"
#include "CodebookName.h"
#include "MetadataTag.h"

#include <optional>
#include <string>
#include <string_view>

namespace dnv::vista::sdk
{
    struct CodebookDto;

    /**
     * @enum PositionValidationResult
     * @brief Result codes for position string validation
     * @details Values < 100 indicate errors (invalid), values >= 100 indicate acceptable (valid or custom).
     *          Used by Codebook::validatePosition() to determine if a position string is valid.
     */
    enum class PositionValidationResult : std::uint8_t
    {
        Invalid = 0,         ///< Invalid format, whitespace, or non-ISO characters
        InvalidOrder = 1,    ///< Numbers not at end or not alphabetically sorted
        InvalidGrouping = 2, ///< Duplicate groups (except DEFAULT_GROUP)
        Valid = 100,         ///< Standard value, number, or valid composite
        Custom = 101         ///< Custom value not in standard codebook
    };

    /**
     * @class Codebook
     * @brief Represents a VIS codebook with standard values, groups, and validation logic
     * @details A codebook contains validated standard values organized into groups.
     *          It provides factory methods to create MetadataTag instances with automatic
     *          validation and custom tag detection. Special validation logic is provided
     *          for the Position codebook (composite positions, ordering, grouping rules).
     *
     *          Codebook instances are created by the Codebooks class from CodebookDto data.
     */
    class Codebook final
    {
        friend class Codebooks;

    private:
        /**
         * @brief Internal constructor
         * @param dto CodebookDto containing raw codebook data
         * @details Constructs codebook by parsing DTO values, building group mappings,
         *          and extracting standard values and group sets.
         */
        explicit Codebook( const CodebookDto& dto );

    public:
        /** @brief Default constructor */
        Codebook() = delete;

        /** @brief Copy constructor */
        Codebook( const Codebook& ) = default;

        /** @brief Move constructor */
        Codebook( Codebook&& ) noexcept = default;

        /** @brief Destructor */
        ~Codebook() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this object
         */
        Codebook& operator=( const Codebook& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this object
         */
        Codebook& operator=( Codebook&& ) noexcept = default;

        /**
         * @brief Get the codebook name
         * @return The CodebookName enum value
         */
        inline CodebookName name() const noexcept;

        /**
         * @brief Get the set of standard values
         * @return Const reference to HashSet of standard value strings
         */
        inline const HashSet<std::string>& standardValues() const noexcept;

        /**
         * @brief Get the set of group names
         * @return Const reference to HashSet of group name strings
         */
        inline const HashSet<std::string>& groups() const noexcept;

        /**
         * @brief Check if a group exists in this codebook
         * @param group Group name to check
         * @return True if group exists
         */
        inline bool hasGroup( std::string_view group ) const noexcept;

        /**
         * @brief Check if a value is a standard value in this codebook
         * @param value Value to check
         * @return True if value is a standard value (for Position codebook, numbers always return true)
         */
        inline bool hasStandardValue( std::string_view value ) const noexcept;

        /**
         * @brief Create a metadata tag with validation
         * @param value The tag value to validate
         * @return Optional MetadataTag if value is valid, std::nullopt otherwise
         */
        [[nodiscard]] std::optional<MetadataTag> createTag( std::string_view value ) const noexcept;

        /**
         * @brief Validate a position string with specific rules
         * @param position The position string to validate
         * @return PositionValidationResult indicating validation outcome
         * @details Validates position strings according to VIS rules:
         *          - ISO string characters only (RFC 3986 unreserved)
         *          - No leading/trailing whitespace
         *          - Standard values and numbers are Valid
         *          - Single non-standard values are Custom
         *          - Composite positions (with '-') must have:
         *            * All parts individually valid
         *            * Numbers only at the end
         *            * Non-numbers alphabetically sorted
         *            * No duplicate groups (except DEFAULT_GROUP)
         * @warning Only valid for Position codebook (undefined behavior for others)
         */
        PositionValidationResult validatePosition( std::string_view position ) const noexcept;

    private:
        CodebookName m_name;                          ///< Codebook type identifier
        HashMap<std::string, std::string> m_groupMap; ///< Maps value -> group name
        HashSet<std::string> m_standardValues;        ///< Set of standard values
        HashSet<std::string> m_groups;                ///< Set of group names
    };
} // namespace dnv::vista::sdk

#include "detail/Codebook.inl"
