/**
 * @file EmbeddedResource.cpp
 * @brief Implementation of resource loading utilities for Vista SDK
 */

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/Serializer.h>
#include <nfx/string/StringBuilder.h>
#include <nfx/string/Utils.h>
#include <zlib-ng.h>

#include "EmbeddedResource.h"

#include "internal/dto/CodebooksDto.h"
#include "internal/dto/GmodDto.h"
#include "internal/dto/GmodVersioningDto.h"
#include "internal/dto/Keys.h"
#include "internal/dto/LocationsDto.h"
#include "internal/transport/dto/ISO19848Dtos.h"

namespace dnv::vista::sdk::internal
{
	namespace
	{
		//=====================================================================
		// Constants
		//=====================================================================

		// Compression buffer sizes for zlib streaming decompression
		static constexpr size_t CHUNK_IN_SIZE = 65536;	 // 64KB input buffer for reading compressed data
		static constexpr size_t CHUNK_OUT_SIZE = 131072; // 128KB output buffer for decompressed data

		// zlib window bits configuration for gzip format
		// 15 = maximum window size (32KB), +16 = gzip format detection
		static constexpr int GZIP_MAX_WINDOW_BITS = 15 + 16;

		//=====================================================================
		// Resource directory configuration
		//=====================================================================

		/**
		 * @brief Get the configured resource directory path
		 * @return The resource directory path to use for loading resources
		 */
		static std::filesystem::path resourceDirectory()
		{
#ifdef VISTA_SDK_RESOURCES_DIR
			// Use compile-time configured resource directory
			return std::filesystem::path{ VISTA_SDK_RESOURCES_DIR };
#else
			// Default: resources subdirectory relative to current working directory
			return std::filesystem::current_path() / "resources";
#endif
		}

		//=====================================================================
		// Resource type checking utilities
		//=====================================================================

		static bool isGmodResource( std::string_view filename ) noexcept
		{
			return nfx::string::contains( filename, "gmod" ) && !nfx::string::contains( filename, "versioning" ) && nfx::string::endsWith( filename, ".json.gz" );
		}

		static bool isGmodVersioningResource( std::string_view filename ) noexcept
		{
			return nfx::string::contains( filename, "gmod" ) && nfx::string::contains( filename, "versioning" ) && nfx::string::endsWith( filename, ".json.gz" );
		}

		static bool isLocationsResource( std::string_view filename ) noexcept
		{
			return nfx::string::contains( filename, "locations" ) && nfx::string::endsWith( filename, ".json.gz" );
		}

		static bool containsVersion( std::string_view filename, std::string_view version ) noexcept
		{
			return nfx::string::contains( filename, version );
		}

		static bool isDataChannelTypeNamesResource( std::string_view filename ) noexcept
		{
			return nfx::string::contains( filename, "data-channel-type-names" ) && nfx::string::contains( filename, "iso19848" ) && nfx::string::endsWith( filename, ".json.gz" );
		}

		static bool isFormatDataTypesResource( std::string_view filename ) noexcept
		{
			return nfx::string::contains( filename, "format-data-types" ) && nfx::string::contains( filename, "iso19848" ) && nfx::string::endsWith( filename, ".json.gz" );
		}
	} // namespace

	//=====================================================================
	// EmbeddedResource class
	//=====================================================================

	//----------------------------------------------
	// Public interface
	//----------------------------------------------

	std::vector<std::string> EmbeddedResource::visVersions()
	{
		auto names = resourceNames();
		std::vector<std::string> visVersions;
		visVersions.reserve( names.size() );

		for ( const auto& resourceName : names )
		{
			if ( isGmodResource( resourceName ) )
			{
				try
				{
					const auto stream = decompressedStream( resourceName );

					std::string jsonContent;
					std::string line;
					while ( std::getline( *stream, line ) )
					{
						jsonContent += line;
					}

					auto docOpt = nfx::serialization::json::Document::fromString( jsonContent );
					if ( !docOpt.has_value() )
					{
						std::fprintf(
							stderr,
							"ERROR: Failed to parse JSON in GMOD resource %s\n",
							resourceName.data() );
						continue;
					}

					auto versionOpt = docOpt->get<std::string>( "/" + std::string{ dto::KEY_VIS_RELEASE } );
					if ( versionOpt.has_value() )
					{
						std::string version = *versionOpt;
						visVersions.push_back( version );
					}
					else
					{
						std::fprintf(
							stderr,
							"WARN: GMOD resource %s missing or has invalid '%.*s' field.\n",
							resourceName.data(),
							static_cast<int>( dto::KEY_VIS_RELEASE.size() ), dto::KEY_VIS_RELEASE.data() );
					}
				}
				catch ( [[maybe_unused]] const std::runtime_error& ex )
				{
					std::fprintf(
						stderr,
						"ERROR: JSON parse error in resource %s: %s\n",
						resourceName.data(),
						ex.what() );
				}
				catch ( [[maybe_unused]] const std::exception& ex )
				{
					std::fprintf(
						stderr,
						"ERROR: Error reading/decompressing resource %s for version extraction: %s\n",
						resourceName.data(),
						ex.what() );
				}
			}
		}

		std::sort( visVersions.begin(), visVersions.end() );
		visVersions.erase( std::unique( visVersions.begin(), visVersions.end() ), visVersions.end() );

		{
			std::ostringstream versionsList;
			if ( !visVersions.empty() )
			{
				std::copy( visVersions.begin(), visVersions.end() - 1, std::ostream_iterator<std::string>( versionsList, ", " ) );
				versionsList << visVersions.back();
			}
			else
			{
				std::fprintf( stderr, "WARN: No VIS versions found in embedded GMOD resources.\n" );
			}
		}

		return visVersions;
	}

	std::optional<nfx::containers::FastHashMap<std::string, GmodVersioningDto>> EmbeddedResource::gmodVersioning()
	{
		auto names = resourceNames();

		std::vector<std::string> matchingResources;
		matchingResources.reserve( names.size() / 4 );

		for ( const auto& resourceName : names )
		{
			if ( isGmodVersioningResource( resourceName ) )
			{
				matchingResources.push_back( resourceName );
			}
		}

		nfx::containers::FastHashMap<std::string, GmodVersioningDto> resultMap;
		bool foundAnyResource = false;

		for ( const auto& resourceName : matchingResources )
		{
			try
			{
				const auto stream = decompressedStream( resourceName );

				std::string jsonContent;
				std::string line;
				while ( std::getline( *stream, line ) )
				{
					jsonContent += line;
				}

				auto docOpt = nfx::serialization::json::Document::fromString( jsonContent );
				if ( !docOpt.has_value() )
				{
					std::fprintf(
						stderr,
						"ERROR: Failed to parse JSON in GMOD Versioning resource %s\n",
						resourceName.data() );
					continue;
				}

				auto visVersionOpt = docOpt->get<std::string>( "/" + std::string{ dto::KEY_VIS_RELEASE } );
				if ( !visVersionOpt.has_value() )
				{
					std::fprintf(
						stderr,
						"WARN: GMOD Versioning resource %s missing or has invalid '%.*s' field.\n",
						resourceName.data(),
						static_cast<int>( dto::KEY_VIS_RELEASE.size() ), dto::KEY_VIS_RELEASE.data() );
					continue;
				}

				std::string visVersion = *visVersionOpt;
				auto dto = GmodVersioningDto::fromJsonString( jsonContent );

				resultMap.insertOrAssign( visVersion, std::move( dto ) );
				foundAnyResource = true;
			}
			catch ( [[maybe_unused]] const std::invalid_argument& ex )
			{
				std::fprintf(
					stderr,
					"ERROR: JSON validation/deserialization error in GMOD Versioning resource %s: %s\n",
					resourceName.data(),
					ex.what() );
			}
			catch ( [[maybe_unused]] const std::runtime_error& ex )
			{
				std::fprintf(
					stderr,
					"ERROR: JSON parse error in GMOD Versioning resource %s: %s\n",
					resourceName.data(),
					ex.what() );
			}
			catch ( [[maybe_unused]] const std::exception& ex )
			{
				std::fprintf(
					stderr,
					"ERROR: Error processing GMOD Versioning resource %s: %s\n",
					resourceName.data(),
					ex.what() );
			}
		}

		if ( foundAnyResource )
		{
			return std::make_optional( std::move( resultMap ) );
		}
		else
		{
			return std::nullopt;
		}
	}

	std::optional<GmodDto> EmbeddedResource::gmod( std::string_view visVersion )
	{
		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(), [&visVersion]( const std::string& name ) {
			return nfx::string::contains( name, "gmod" ) && nfx::string::contains( name, visVersion ) &&
				   nfx::string::endsWith( name, ".json.gz" ) &&
				   !nfx::string::contains( name, "versioning" );
		} );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: GMOD resource not found for version: %.*s\n",
				static_cast<int>( visVersion.size() ), visVersion.data() );
			return std::nullopt;
		}

		try
		{
			auto stream = decompressedStream( *it );

			std::string jsonContent;
			std::string line;
			while ( std::getline( *stream, line ) )
			{
				jsonContent += line;
			}

			GmodDto loadedDto = GmodDto::fromJsonString( jsonContent );

			return std::make_optional( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const std::invalid_argument& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in GMOD resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::runtime_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON parse error in GMOD resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing GMOD resource %s: %s\n",
				it->c_str(), ex.what() );
		}

		return std::nullopt;
	}

	std::optional<CodebooksDto> EmbeddedResource::codebooks( std::string_view visVersion )
	{
		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(), [&visVersion]( const std::string& name ) {
			return nfx::string::contains( name, "codebooks" ) &&
				   nfx::string::contains( name, visVersion ) &&
				   nfx::string::endsWith( name, ".json.gz" );
		} );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: Codebooks resource not found for version: %.*s\n",
				static_cast<int>( visVersion.size() ), visVersion.data() );
			return std::nullopt;
		}

		try
		{
			auto stream = decompressedStream( *it );

			std::string jsonContent;
			std::string line;
			while ( std::getline( *stream, line ) )
			{
				jsonContent += line;
			}

			CodebooksDto loadedDto = CodebooksDto::fromJsonString( jsonContent );

			return std::make_optional( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const std::invalid_argument& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in Codebooks resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::runtime_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON parse error in Codebooks resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing Codebooks resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}

		return std::nullopt;
	}

	std::optional<LocationsDto> EmbeddedResource::locations( std::string_view visVersion )
	{
		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(),
			[&visVersion]( const std::string& name ) { return isLocationsResource( name ) &&
															  containsVersion( name, visVersion ); } );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: Locations resource not found for version: %.*s\n",
				static_cast<int>( visVersion.size() ), visVersion.data() );
			return std::nullopt;
		}

		try
		{
			auto stream = decompressedStream( *it );

			std::string jsonContent;
			std::string line;
			while ( std::getline( *stream, line ) )
			{
				jsonContent += line;
			}

			LocationsDto loadedDto = LocationsDto::fromJsonString( jsonContent );

			return std::make_optional( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const std::invalid_argument& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in Locations resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::runtime_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON parse error in Locations resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing Locations resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}

		return std::nullopt;
	}

	std::optional<transport::DataChannelTypeNamesDto> EmbeddedResource::dataChannelTypeNames( std::string_view version )
	{
		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(),
			[&version]( const std::string& name ) { return isDataChannelTypeNamesResource( name ) &&
														   containsVersion( name, version ); } );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: DataChannelTypeNames resource not found for version: %.*s\n",
				static_cast<int>( version.size() ), version.data() );
			return std::nullopt;
		}

		try
		{
			auto stream = decompressedStream( *it );

			std::string jsonContent;
			std::string line;
			while ( std::getline( *stream, line ) )
			{
				jsonContent += line;
			}

			transport::DataChannelTypeNamesDto loadedDto = transport::DataChannelTypeNamesDto::fromJsonString( jsonContent );

			return std::make_optional( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const std::invalid_argument& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in DataChannelTypeNames resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::runtime_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON parse error in DataChannelTypeNames resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing DataChannelTypeNames resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}

		return std::nullopt;
	}

	std::optional<transport::FormatDataTypesDto> EmbeddedResource::formatDataTypes( std::string_view version )
	{
		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(),
			[&version]( const std::string& name ) { return isFormatDataTypesResource( name ) &&
														   containsVersion( name, version ); } );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: FormatDataTypes resource not found for version: %.*s\n",
				static_cast<int>( version.size() ), version.data() );
			return std::nullopt;
		}

		try
		{
			auto stream = decompressedStream( *it );

			std::string jsonContent;
			std::string line;
			while ( std::getline( *stream, line ) )
			{
				jsonContent += line;
			}

			transport::FormatDataTypesDto loadedDto = transport::FormatDataTypesDto::fromJsonString( jsonContent );

			return std::make_optional( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const std::invalid_argument& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in FormatDataTypes resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::runtime_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON parse error in FormatDataTypes resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing FormatDataTypes resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}

		return std::nullopt;
	}

	//----------------------------------------------
	// Resource access
	//----------------------------------------------

	std::vector<std::string> EmbeddedResource::resourceNames()
	{
		const auto resourcesDir = resourceDirectory();

		std::vector<std::string> foundNames;
		foundNames.reserve( 50 );

		try
		{
			if ( !std::filesystem::exists( resourcesDir ) || !std::filesystem::is_directory( resourcesDir ) )
			{
				throw std::runtime_error{ "Resources directory not found: " + resourcesDir.string() };
			}

			for ( const auto& entry : std::filesystem::directory_iterator( resourcesDir ) )
			{
				if ( entry.is_regular_file() && nfx::string::endsWith( entry.path().filename().string(), ".json.gz" ) )
				{
					foundNames.emplace_back( entry.path().filename().string() );
				}
			}

			if ( foundNames.empty() )
			{
				std::fprintf(
					stderr,
					"WARN: No embedded resource files (.json.gz) found in resources directory: %s\n",
					resourcesDir.string().c_str() );
			}
		}
		catch ( [[maybe_unused]] const std::filesystem::filesystem_error& ex )
		{
			throw std::runtime_error{ "Filesystem error scanning resources directory " + resourcesDir.string() + ": " + ex.what() };
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			throw std::runtime_error{ "Error scanning resources directory " + resourcesDir.string() + ": " + ex.what() };
		}

		return foundNames;
	}

	std::shared_ptr<std::istream> EmbeddedResource::decompressedStream( std::string_view resourceName )
	{
		auto compressedStream = stream( resourceName );

		if ( !compressedStream || !compressedStream->good() )
		{
			throw std::runtime_error{ "Failed to open compressed stream for resource: " + std::string{ resourceName } };
		}

		compressedStream->seekg( 0, std::ios::end );
		auto pos = compressedStream->tellg();
		if ( pos < 0 )
		{
			throw std::runtime_error{ "Invalid stream position for resource: " + std::string{ resourceName } };
		}
		size_t compressedSize = static_cast<size_t>( pos );
		compressedStream->seekg( 0, std::ios::beg );

		size_t estimatedDecompressedSize = std::min( compressedSize * 8, static_cast<size_t>( 128 * 1024 * 1024 ) );

		std::string decompressedData;
		decompressedData.reserve( estimatedDecompressedSize );

		zng_stream zs = {};
		zs.zalloc = nullptr;
		zs.zfree = nullptr;
		zs.opaque = nullptr;

		if ( ::zng_inflateInit2( &zs, GZIP_MAX_WINDOW_BITS ) != 0 )
		{
			throw std::runtime_error{ "Failed to initialize zlib for resource: " + std::string{ resourceName } };
		}

		std::vector<char> inBuffer( CHUNK_IN_SIZE );
		std::vector<char> outBuffer( CHUNK_OUT_SIZE );
		int ret = Z_OK;

		do
		{
			compressedStream->read( inBuffer.data(), CHUNK_IN_SIZE );
			zs.avail_in = static_cast<uInt>( compressedStream->gcount() );

			if ( zs.avail_in == 0 && !compressedStream->eof() )
			{
				::zng_inflateEnd( &zs );
				throw std::runtime_error{ "Error reading compressed stream for resource: " + std::string{ resourceName } };
			}

			zs.next_in = reinterpret_cast<Bytef*>( inBuffer.data() );

			do
			{
				zs.avail_out = CHUNK_OUT_SIZE;
				zs.next_out = reinterpret_cast<Bytef*>( outBuffer.data() );

				ret = ::zng_inflate( &zs, Z_NO_FLUSH );

				if ( ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR )
				{
					auto errorMsg = ( zs.msg ) ? zs.msg : "Unknown zlib error";
					::zng_inflateEnd( &zs );

					// Use fprintf for the numeric error code to avoid append issues
					char errorBuffer[512];
					std::snprintf( errorBuffer, sizeof( errorBuffer ),
						"Zlib decompression failed for resource '%.*s' with error code %d: %s",
						static_cast<int>( resourceName.size() ), resourceName.data(),
						ret, errorMsg );
					throw std::runtime_error( errorBuffer );
				}

				size_t have = CHUNK_OUT_SIZE - zs.avail_out;
				decompressedData.append( outBuffer.data(), have );
			} while ( zs.avail_out == 0 );
		} while ( ret != Z_STREAM_END );

		::zng_inflateEnd( &zs );

		auto decompressedBuffer = std::make_shared<std::istringstream>( std::move( decompressedData ) );
		return decompressedBuffer;
	}

	std::shared_ptr<std::istream> EmbeddedResource::stream( std::string_view resourceName )
	{
		const auto resourcePath = resourceDirectory() / resourceName;

		try
		{
			if ( std::filesystem::exists( resourcePath ) && std::filesystem::is_regular_file( resourcePath ) )
			{
				auto fileStream = std::make_shared<std::ifstream>( resourcePath, std::ios::binary );
				if ( fileStream->is_open() )
				{
					return fileStream;
				}
				else
				{
					throw std::runtime_error( "Found resource '" + resourcePath.string() + "' but failed to open stream" );
				}
			}
		}
		catch ( [[maybe_unused]] const std::filesystem::filesystem_error& ex )
		{
			throw std::runtime_error( "Filesystem error accessing resource '" + resourcePath.string() + "': " + ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			throw std::runtime_error( "Error accessing resource '" + resourcePath.string() + "': " + ex.what() );
		}

		throw std::runtime_error( "Resource file not found: " + resourcePath.string() );
	}
} // namespace dnv::vista::sdk::internal
