/**
 * @file EmbeddedTestData.h
 * @brief Access embedded test data files for unit tests
 * @details Provides access to testdata/ *.json, testdata/ *.txt, etc. embedded at build time
 */

#pragma once

#include <nfx/Serialization.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
    /**
     * @brief Access to embedded test data resources
     *
     * This class provides access to testdata/ files embedded at compile time.
     *
     * Example:
     * @code
     *   // Load and deserialize Codebook test data
     *   auto testData = EmbeddedTestData::load<CodebookTestDataDto>("Codebook.json");
     *
     *   // List all available test files
     *   auto files = EmbeddedTestData::listFiles();
     * @endcode
     */
    class EmbeddedTestData
    {
    public:
        /**
         * @brief List all available test data filenames
         * @return Vector of filenames (e.g., ["Codebook.json", "LocalIds.txt", ...])
         */
        static std::vector<std::string> listFiles();

        /**
         * @brief Load and deserialize JSON test data to DTO
         * @tparam TDto DTO type to deserialize into
         * @param filename Name of the JSON file (without path, e.g., "Codebook.json")
         * @return Deserialized DTO, or std::nullopt if not found or parse error
         */
        template <typename TDto>
        static std::optional<TDto> load( std::string_view filename );

    private:
        static std::vector<uint8_t> getBytes( std::string_view filename );
        static std::string getText( std::string_view filename );
    };

    template <typename TDto>
    std::optional<TDto> EmbeddedTestData::load( std::string_view filename )
    {
        auto text = getText( filename );
        if( text.empty() )
        {
            return std::nullopt;
        }

        // Special case: std::string just returns the text directly
        if constexpr( std::is_same_v<TDto, std::string> )
        {
            return text;
        }

        // Special case: nfx::json::Document just parses JSON directly
        if constexpr( std::is_same_v<TDto, nfx::json::Document> )
        {
            return nfx::json::Document::fromString( text );
        }
        else
        {
            // Other DTOs use Serializer
            try
            {
                return nfx::serialization::json::Serializer<TDto>::fromString( text );
            }
            catch( ... )
            {
                return std::nullopt;
            }
        }
    }
} // namespace dnv::vista::sdk
