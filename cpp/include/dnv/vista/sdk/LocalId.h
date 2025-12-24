/**
 * @file LocalId.h
 * @brief Immutable VIS Local ID representation
 * @details Provides an immutable LocalId class representing a unique sensor identifier
 *          according to the dnv-v2 naming rule. LocalId combines GmodPath(s) with
 *          metadata tags to create globally unique sensor identifiers within a vessel.
 *          Format: /dnv-v2/vis-3-4a/411.1/H101/meta/qty-power/cnt-fuel.oil
 */

#pragma once

#include "CodebookName.h"
#include "GmodPath.h"
#include "LocalIdBuilder.h"
#include "MetadataTag.h"
#include "VisVersions.h"

#include <optional>
#include <string>
#include <vector>

namespace dnv::vista::sdk
{
    class ParsingErrors;

    /**
     * @class LocalId
     * @brief Immutable representation of a VIS Local ID
     * @details Represents a unique sensor identifier within a vessel according to the dnv-v2 naming rule.
     *          LocalId is constructed from a LocalIdBuilder and provides read-only access to all components.
     *          Once constructed, a LocalId cannot be modified. To create variations, access the builder
     *          via builder() and create a new LocalId instance.
     *
     * @note LocalId follows the dnv-v2 naming rule format:
     *       /dnv-v2/vis-{version}/{primary-item}[/sec/{secondary-item}]/meta[/qty-{value}][/cnt-{value}]...
     */
    class LocalId
    {
        friend class LocalIdBuilder;

    protected:
        /**
         * @brief Construct LocalId from builder
         * @param builder LocalIdBuilder with validated state
         * @throws std::invalid_argument if builder is invalid (missing primary item)
         */
        explicit LocalId( const LocalIdBuilder& builder );

    public:
        /** @brief Default constructor */
        LocalId() = delete;

        /** @brief Copy constructor */
        LocalId( const LocalId& ) = default;

        /** @brief Move constructor */
        LocalId( LocalId&& ) noexcept = default;

        /** @brief Destructor */
        virtual ~LocalId() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this object
         */
        LocalId& operator=( const LocalId& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this object
         */
        LocalId& operator=( LocalId&& ) noexcept = default;

        /**
         * @brief Check equality with another LocalId
         * @param other LocalId to compare with
         * @return True if all components are equal
         */
        [[nodiscard]] inline bool operator==( const LocalId& other ) const noexcept;

        /**
         * @brief Check inequality with another LocalId
         * @param other LocalId to compare with
         * @return True if any component differs
         */
        [[nodiscard]] inline bool operator!=( const LocalId& other ) const noexcept;

        /**
         * @brief Get ISO 19848 Annex C naming rule identifier
         * @return String view containing the naming rule identifier ("dnv-v2")
         */
        [[nodiscard]] static constexpr std::string_view namingRule() noexcept;

        /**
         * @brief Get the VIS version for this LocalId
         * @return The VisVersion enum value
         */
        [[nodiscard]] inline VisVersion version() const noexcept;

        /**
         * @brief Get the primary item (Gmod path)
         * @return Reference to the primary GmodPath
         */
        [[nodiscard]] inline const GmodPath& primaryItem() const noexcept;

        /**
         * @brief Get the secondary item (Gmod path)
         * @return Optional containing the secondary GmodPath if set, nullopt otherwise
         */
        [[nodiscard]] inline const std::optional<GmodPath>& secondaryItem() const noexcept;

        /**
         * @brief Get the verbose mode flag
         * @return True if verbose mode is enabled
         */
        [[nodiscard]] inline bool isVerboseMode() const noexcept;

        /**
         * @brief Get all metadata tags that are set
         * @return Vector of all non-empty metadata tags
         */
        [[nodiscard]] inline std::vector<MetadataTag> metadataTags() const;

        /**
         * @brief Get the Quantity metadata tag
         * @return Optional containing the Quantity tag if set, nullopt otherwise
         */
        [[nodiscard]] inline const std::optional<MetadataTag>& quantity() const noexcept;

        /**
         * @brief Get the Content metadata tag
         * @return Optional containing the Content tag if set, nullopt otherwise
         */
        [[nodiscard]] inline const std::optional<MetadataTag>& content() const noexcept;

        /**
         * @brief Get the Calculation metadata tag
         * @return Optional containing the Calculation tag if set, nullopt otherwise
         */
        [[nodiscard]] inline const std::optional<MetadataTag>& calculation() const noexcept;

        /**
         * @brief Get the State metadata tag
         * @return Optional containing the State tag if set, nullopt otherwise
         */
        [[nodiscard]] inline const std::optional<MetadataTag>& state() const noexcept;

        /**
         * @brief Get the Command metadata tag
         * @return Optional containing the Command tag if set, nullopt otherwise
         */
        [[nodiscard]] inline const std::optional<MetadataTag>& command() const noexcept;

        /**
         * @brief Get the Type metadata tag
         * @return Optional containing the Type tag if set, nullopt otherwise
         */
        [[nodiscard]] inline const std::optional<MetadataTag>& type() const noexcept;

        /**
         * @brief Get the Position metadata tag
         * @return Optional containing the Position tag if set, nullopt otherwise
         */
        [[nodiscard]] inline const std::optional<MetadataTag>& position() const noexcept;

        /**
         * @brief Get the Detail metadata tag
         * @return Optional containing the Detail tag if set, nullopt otherwise
         */
        [[nodiscard]] inline const std::optional<MetadataTag>& detail() const noexcept;

        /**
         * @brief Check if LocalId has any custom (non-standard) metadata tags
         * @return True if any metadata tag is custom
         */
        [[nodiscard]] inline bool hasCustomTag() const noexcept;

        /**
         * @brief Get the builder that created this LocalId
         * @return Const reference to the internal builder
         */
        [[nodiscard]] inline const LocalIdBuilder& builder() const noexcept;

        /**
         * @brief Convert to string representation (lvalue-qualified)
         * @return String representation of the LocalId in dnv-v2 format
         */
        [[nodiscard]] virtual inline std::string toString() const&;

        /**
         * @brief Convert to string representation (rvalue-qualified)
         * @return String representation with potential move optimization
         */
        [[nodiscard]] virtual inline std::string toString() &&;

        /**
         * @brief Append string representation to StringBuilder
         * @param sb StringBuilder to append to
         */
        virtual void inline toString( StringBuilder& sb ) const;

        /**
         * @brief Create LocalId from string representation
         * @param localIdStr String representation of LocalId in dnv-v2 format
         * @return Optional containing LocalId if parsing succeeded, nullopt otherwise
         * @note This method silently fails on parse errors. Use the overload with ParsingErrors for diagnostics.
         * @note VIS version is extracted from the localIdStr (e.g., /dnv-v2/vis-3-4a/...)
         */
        [[nodiscard]] static std::optional<LocalId> fromString( std::string_view localIdStr ) noexcept;

        /**
         * @brief Create LocalId from string representation with error reporting
         * @param localIdStr String representation of LocalId in dnv-v2 format
         * @param errors Output parameter filled with parsing errors if parsing fails
         * @return Optional containing LocalId if parsing succeeded, nullopt otherwise
         * @note VIS version is extracted from the localIdStr (e.g., /dnv-v2/vis-3-4a/...)
         */
        [[nodiscard]] static std::optional<LocalId> fromString(
            std::string_view localIdStr, ParsingErrors& errors ) noexcept;

    protected:
        LocalIdBuilder m_builder; ///< Internal builder containing all state
    };
} // namespace dnv::vista::sdk

namespace dnv::vista::sdk::mqtt
{
    /**
     * @brief MQTT-compatible LocalId implementation inheriting from main LocalId.
     * @details This class extends the base LocalId with MQTT-specific formatting capabilities:
     *          - Underscores instead of slashes in Gmod paths for topic compatibility
     *          - No leading slash to match MQTT topic conventions
     *          - No "meta/" prefix section for cleaner IoT topics
     *          - Placeholder handling for missing components
     */
    class LocalId final : public sdk::LocalId
    {
    public:
        /**
         * @brief Constructs MQTT LocalId from validated LocalIdBuilder.
         * @param[in] builder Valid LocalIdBuilder instance.
         * @throws std::invalid_argument If builder is invalid or empty.
         */
        explicit LocalId( const sdk::LocalIdBuilder& builder );

        /** @brief Default constructor */
        LocalId() = delete;

        /**
         * @brief Copy constructor
         * @param other The MQTT LocalId to copy from
         */
        LocalId( const LocalId& other ) = default;

        /**
         * @brief Move constructor
         * @param other The MQTT LocalId to move from
         */
        LocalId( LocalId&& other ) noexcept = default;

        /** @brief Destructor */
        virtual ~LocalId() override = default;

        /**
         * @brief Copy assignment operator
         */
        LocalId& operator=( const LocalId& ) = delete;

        /**
         * @brief Move assignment operator
         * @param other The MQTT LocalId to move from
         * @return Reference to this LocalId
         */
        LocalId& operator=( LocalId&& other ) noexcept = default;

        /**
         * @brief Converts LocalId to MQTT-compatible topic string.
         * @details Provides MQTT-specific formatting:
         *          - No leading slash
         *          - Underscores instead of slashes in paths
         *          - No "meta/" section
         *          - Placeholders for missing components
         * @return MQTT-compatible Local ID topic string.
         */
        [[nodiscard]] virtual inline std::string toString() const& override;

        /**
         * @brief Convert to MQTT topic string (rvalue-qualified)
         * @return MQTT-compatible string with potential move optimization
         */
        [[nodiscard]] virtual inline std::string toString() && override;

        /**
         * @brief Append MQTT topic string to StringBuilder
         * @param sb StringBuilder to append to
         */
        virtual inline void toString( StringBuilder& sb ) const override;

    private:
        /** @brief Internal separator for MQTT paths */
        static constexpr char m_separator = '_';

        /**
         * @brief Appends Gmod path to builder with MQTT formatting.
         * @param[in,out] builder StringBuilder to append to.
         * @param[in] path GmodPath to append with underscore separators.
         */
        inline void appendPath( StringBuilder& builder, const GmodPath& path ) const;

        /**
         * @brief Appends primary item to builder in MQTT format.
         * @param[in] localIdBuilder LocalIdBuilder containing the primary item.
         * @param[in,out] builder StringBuilder to append to.
         */
        inline void appendPrimaryItem( const LocalIdBuilder& localIdBuilder, StringBuilder& builder ) const;

        /**
         * @brief Appends secondary item or placeholder to builder in MQTT format.
         * @param[in] localIdBuilder LocalIdBuilder containing the optional secondary item.
         * @param[in,out] builder StringBuilder to append to.
         */
        inline void appendSecondaryItem( const LocalIdBuilder& localIdBuilder, StringBuilder& builder ) const;

        /**
         * @brief Appends metadata tag or placeholder to builder in MQTT format.
         * @details Appends the metadata tag value if present, otherwise appends an underscore placeholder.
         *          Always appends a trailing forward slash for MQTT topic formatting.
         * @param[in,out] builder StringBuilder to append to.
         * @param[in] tag Optional metadata tag to append (or placeholder if empty).
         */
        inline void appendMeta( StringBuilder& builder, const std::optional<MetadataTag>& tag ) const;
    };
} // namespace dnv::vista::sdk::mqtt

#include "detail/LocalId.inl"
