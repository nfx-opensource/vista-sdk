/**
 * @file CustomDictionary.h
 * @brief Custom dictionary wrapper for JSON documents with automatic type serialization
 */

#pragma once

#include "dnv/vista/sdk/serialization/Serialization.h"

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace dnv::vista::sdk::transport
{
    /**
     * @brief Custom dictionary wrapper for extensible JSON data
     * @details Wraps json::Document to provide:
     *          - Direct support for JSON native types (string, int64_t, double, bool)
     *          - Automatic serialization for custom types (Decimal, DateTime, etc.)
     *          - Type-safe get/set operations with SFINAE-based overload resolution
     */
    class CustomDictionary final
    {
    public:
        /** @brief Default constructor */
        CustomDictionary() = default;

        /** @brief Copy constructor */
        CustomDictionary( const CustomDictionary& ) = default;

        /** @brief Move constructor */
        CustomDictionary( CustomDictionary&& ) noexcept = default;

        /** @brief Destructor */
        ~CustomDictionary() = default;

        /**
         * @brief Copy assignment
         * @return Reference to this object
         */
        CustomDictionary& operator=( const CustomDictionary& ) = default;

        /**
         * @brief Move assignment
         * @return Reference to this object
         */
        CustomDictionary& operator=( CustomDictionary&& ) noexcept = default;

        /**
         * @brief Set a value for a key (JSON native types)
         * @tparam T Type of the value (string, int64_t, double, bool)
         * @param key The key string
         * @param value The value to store
         */
        template <typename T>
            requires(
                std::is_same_v<T, std::string> || std::is_same_v<T, int64_t> || std::is_same_v<T, double> ||
                std::is_same_v<T, bool> )
        void set( std::string_view key, T value )
        {
            m_doc.set( std::string( key ), json::Document{ value } );
        }

        /**
         * @brief Set a serializable value for a key (custom types via Serializer)
         * @tparam T Serializable type (Decimal, DateTime, etc.)
         * @param key The key string
         * @param value The value to store
         */
        template <typename T>
            requires(
                !std::is_same_v<T, std::string> && !std::is_same_v<T, int64_t> && !std::is_same_v<T, double> &&
                !std::is_same_v<T, bool> )
        void set( std::string_view key, const T& value )
        {
            auto str = serialization::json::Serializer<T>::toString( value );
            auto valueDoc = json::Document::fromString( str );
            if( valueDoc )
            {
                m_doc.set( std::string( key ), *valueDoc );
            }
        }

        /**
         * @brief Get a value by key (JSON native types)
         * @tparam T Type of the value (string, int64_t, double, bool)
         * @param key The key string
         * @return Optional containing the value if found
         */
        template <typename T>
            requires(
                std::is_same_v<T, std::string> || std::is_same_v<T, int64_t> || std::is_same_v<T, double> ||
                std::is_same_v<T, bool> )
        [[nodiscard]] std::optional<T> get( std::string_view key ) const
        {
            return m_doc.get<T>( key );
        }

        /**
         * @brief Get a serializable value by key (custom types via Serializer)
         * @tparam T Serializable type (Decimal, DateTime, etc.)
         * @param key The key string
         * @return Optional containing the deserialized value if found
         */
        template <typename T>
            requires(
                !std::is_same_v<T, std::string> && !std::is_same_v<T, int64_t> && !std::is_same_v<T, double> &&
                !std::is_same_v<T, bool> )
        [[nodiscard]] std::optional<T> get( std::string_view key ) const
        {
            auto docOpt = m_doc.get<json::Document>( key );
            if( !docOpt )
                return std::nullopt;

            T result{};
            serialization::json::SerializationTraits<T>::fromDocument( *docOpt, result );
            return result;
        }

        /**
         * @brief Get the underlying Document (for serialization)
         * @return Reference to the underlying json::Document
         */
        [[nodiscard]] const json::Document& document() const noexcept
        {
            return m_doc;
        }

        /**
         * @brief Get the underlying Document (for serialization)
         * @return Reference to the underlying json::Document
         */
        [[nodiscard]] json::Document& document() noexcept
        {
            return m_doc;
        }

    private:
        json::Document m_doc;
    };
} // namespace dnv::vista::sdk::transport
