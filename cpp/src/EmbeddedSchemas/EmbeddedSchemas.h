/**
 * @file EmbeddedSchemas.h
 * @brief Header for embedded schema resources
 * @details Provides compile-time embedded access to JSON and XSD schema files
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
    /**
     * @brief Access to embedded ISO19848 schema files
     * @details Provides compile-time embedded access to JSON and XSD schemas
     */
    class EmbeddedSchema final
    {
    public:
        /**
         * @brief Get DataChannelList JSON schema
         * @return JSON schema as string_view, nullopt if not found
         */
        static std::optional<std::string_view> dataChannelListJson();

        /**
         * @brief Get TimeSeriesData JSON schema
         * @return JSON schema as string_view, nullopt if not found
         */
        static std::optional<std::string_view> timeSeriesDataJson();

        /**
         * @brief Get DataChannelList XSD schema
         * @return XSD schema as string_view, nullopt if not found
         */
        static std::optional<std::string_view> dataChannelListXsd();

        /**
         * @brief Get TimeSeriesData XSD schema
         * @return XSD schema as string_view, nullopt if not found
         */
        static std::optional<std::string_view> timeSeriesDataXsd();

        /**
         * @brief Get schema by name
         * @param name Schema filename (e.g., "DataChannelList.schema.json")
         * @return Schema content as string_view, nullopt if not found
         */
        static std::optional<std::string_view> get( std::string_view name );
    };
} // namespace dnv::vista::sdk
