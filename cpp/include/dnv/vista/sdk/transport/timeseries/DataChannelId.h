/**
 * @file DataChannelId.h
 * @brief Data channel identifier for ISO 19848 time series data
 */

#pragma once

#include "dnv/vista/sdk/LocalId.h"

#include <functional>
#include <optional>
#include <string>
#include <variant>

namespace dnv::vista::sdk::transport::timeseries
{
    //=====================================================================
    // DataChannelId class
    //=====================================================================

    /**
     * @brief Polymorphic data channel identifier supporting LocalId or ShortId
     * @details Type-safe sum type that can contain either:
     *          - LocalId: full VIS local identifier (starts with '/')
     *          - ShortId: short string identifier
     */
    class DataChannelId final
    {
    private:
        /**
         * @brief Constructs DataChannelId from LocalId
         * @param localId Local identifier
         */
        explicit DataChannelId( LocalId localId ) noexcept;

        /**
         * @brief Constructs DataChannelId from string_view identifier
         * @param shortId Short identifier string
         */
        explicit DataChannelId( std::string shortId ) noexcept;

    public:
        /** @brief Default constructor */
        DataChannelId() = delete;

        /** @brief Copy constructor */
        DataChannelId( const DataChannelId& ) = default;

        /** @brief Move constructor */
        DataChannelId( DataChannelId&& ) noexcept = default;

        /** @brief Destructor */
        ~DataChannelId() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this DataChannelId
         */
        DataChannelId& operator=( const DataChannelId& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this DataChannelId
         */
        DataChannelId& operator=( DataChannelId&& ) noexcept = default;

        /**
         * @brief Equality comparison
         * @param other DataChannelId to compare with
         * @return true if equal (same variant type and same value)
         */
        [[nodiscard]] inline bool operator==( const DataChannelId& other ) const noexcept;

        /**
         * @brief Inequality comparison
         * @param other DataChannelId to compare with
         * @return true if not equal
         */
        [[nodiscard]] inline bool operator!=( const DataChannelId& other ) const noexcept;

        /**
         * @brief Check if this contains a LocalId
         * @return true if contains LocalId, false if contains ShortId
         */
        [[nodiscard]] inline bool isLocalId() const noexcept;

        /**
         * @brief Check if this contains a ShortId
         * @return true if contains ShortId, false if contains LocalId
         */
        [[nodiscard]] inline bool isShortId() const noexcept;

        /**
         * @brief Get LocalId if present
         * @return Optional LocalId reference
         */
        [[nodiscard]] inline std::optional<std::reference_wrapper<const LocalId>> localId() const noexcept;

        /**
         * @brief Get ShortId if present
         * @return Optional ShortId string view
         */
        [[nodiscard]] inline std::optional<std::string_view> shortId() const noexcept;

        /**
         * @brief Convert to string representation
         * @return String representation (LocalId.toString() or ShortId)
         */
        [[nodiscard]] inline std::string toString() const;

        /**
         * @brief Pattern matching visitor for DataChannelId
         * @tparam F1 Function type for LocalId case
         * @tparam F2 Function type for ShortId case
         * @param onLocalId Function to call if DataChannelId contains LocalId
         * @param onShortId Function to call if DataChannelId contains ShortId
         * @return Result of the matching function
         */
        template <typename F1, typename F2>
        [[nodiscard]] inline auto match( F1&& onLocalId, F2&& onShortId ) const;

        /**
         * @brief Visit DataChannelId with side effects (no return value)
         * @tparam F1 Function type for LocalId case
         * @tparam F2 Function type for ShortId case
         * @param onLocalId Function to call if DataChannelId contains LocalId
         * @param onShortId Function to call if DataChannelId contains ShortId
         */
        template <typename F1, typename F2>
        inline void visit( F1&& onLocalId, F2&& onShortId ) const;

        /**
         * @brief Create DataChannelId from string representation
         * @param value String to parse (LocalId if starts with '/', otherwise ShortId)
         * @return Optional containing DataChannelId if parsing succeeded, nullopt otherwise
         * @note LocalId parsing errors are silently ignored (returns nullopt)
         */
        [[nodiscard]] static inline std::optional<DataChannelId> fromString( std::string_view value ) noexcept;

    private:
        std::variant<LocalId, std::string> m_value;
    };
} // namespace dnv::vista::sdk::transport::timeseries

#include "dnv/vista/sdk/detail/transport/timeseries/DataChannelId.inl"
