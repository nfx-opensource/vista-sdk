/**
 * @file EmbeddedResource.h
 * @brief Resource loading utilities for Vista SDK
 */

#pragma once

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

#include <nfx/Containers.h>

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	struct GmodDto;
	struct GmodVersioningDto;
	struct CodebooksDto;
	struct LocationsDto;

	namespace internal
	{
		namespace transport
		{
			struct DataChannelTypeNamesDto;
			struct FormatDataTypesDto;
		} // namespace transport

		//=====================================================================
		// EmbeddedResource class
		//=====================================================================

		/**
		 * @brief Utility class for accessing embedded resources
		 *
		 * This class provides access to gzipped JSON resource files containing configuration
		 * data for the Vista SDK.
		 */
		class EmbeddedResource final
		{
		public:
			/**
			 * @brief Get all available VIS versions
			 *
			 * Scans all resources to extract the VIS versions available in the current
			 * resource collection.
			 *
			 * @return Vector of VIS version strings
			 */
			static std::vector<std::string> visVersions();

			/**
			 * @brief Get all GMOD versioning data
			 * @return Dictionary of versioning DTOs by version string with heterogeneous lookup if found, std::nullopt otherwise
			 */
			static std::optional<nfx::containers::FastHashMap<std::string, GmodVersioningDto>> gmodVersioning();

			/**
			 * @brief Get GMOD for specific VIS version
			 * @param visVersion VIS version string
			 * @return GMOD DTO if found, std::nullopt otherwise
			 */
			static std::optional<GmodDto> gmod( std::string_view visVersion );

			/**
			 * @brief Get codebooks for specific VIS version
			 * @param visVersion VIS version string
			 * @return Codebooks DTO if found, std::nullopt otherwise
			 */
			static std::optional<CodebooksDto> codebooks( std::string_view visVersion );

			/**
			 * @brief Get locations for specific VIS version
			 * @param visVersion VIS version string
			 * @return Locations DTO if found, std::nullopt otherwise
			 */
			static std::optional<LocationsDto> locations( std::string_view visVersion );

			/**
			 * @brief Get data channel type names for specific version
			 * @param version ISO 19848 version string
			 * @return DataChannelTypeNamesDto if found, std::nullopt otherwise
			 */
			static std::optional<transport::DataChannelTypeNamesDto> dataChannelTypeNames( std::string_view version );

			/**
			 * @brief Get format data types for specific version
			 * @param version ISO 19848 version string
			 * @return FormatDataTypesDto if found, std::nullopt otherwise
			 */
			static std::optional<transport::FormatDataTypesDto> formatDataTypes( std::string_view version );

		private:
			//----------------------------------------------
			// Resource access
			//----------------------------------------------

			/**
			 * @brief Get all embedded resource names
			 *        Scans predefined directories for resource files with .json.gz extension.
			 * @return Vector of resource names
			 */
			static std::vector<std::string> resourceNames();

			/**
			 * @brief Get decompressed stream for gzipped resource
			 *
			 * Loads a gzipped resource and decompresses it using zlib.
			 *
			 * @param resourceName Compressed resource name
			 * @return Shared pointer to input stream containing the decompressed data
			 * @throws std::runtime_error if decompression fails
			 */
			static std::shared_ptr<std::istream> decompressedStream( std::string_view resourceName );

			/**
			 * @brief Get resource stream for specific resource name
			 *
			 * Searches for the resource file in multiple possible directories and
			 * returns a stream for reading the resource data.
			 *
			 * @param resourceName Resource name
			 * @return Shared pointer to input stream containing the resource data
			 * @throws std::runtime_error if resource cannot be found
			 */
			static std::shared_ptr<std::istream> stream( std::string_view resourceName );
		};
	} // namespace internal
} // namespace dnv::vista::sdk
