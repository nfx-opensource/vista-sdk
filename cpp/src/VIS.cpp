/**
 * @file VIS.cpp
 * @brief Implementation of Vessel Information Structure (VIS) interface
 */

#include <nfx/memory/MemoryCache.h>

#include "dnv/vista/sdk/VIS.h"

#include "internal/core/EmbeddedResource.h"
#include "internal/core/GmodVersioning.h"
#include "internal/dto/CodebooksDto.h"
#include "internal/dto/GmodDto.h"
#include "internal/dto/LocationsDto.h"

#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/Locations.h"

namespace dnv::vista::sdk
{
	namespace internal
	{
		//=====================================================================
		// Internal Cache Infrastructure
		//=====================================================================

		//----------------------------------------------
		// Memory caching
		//----------------------------------------------

		//-----------------------------
		// DTOs
		//-----------------------------

		static auto gmodDtoCache{
			nfx::memory::MemoryCache<VisVersion, GmodDto>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto codebooksDtoCache{
			nfx::memory::MemoryCache<VisVersion, CodebooksDto>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto locationsDtoCache{
			nfx::memory::MemoryCache<VisVersion, LocationsDto>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto gmodVersioningDtoCache{
			nfx::memory::MemoryCache<VisVersion, nfx::containers::StringMap<GmodVersioningDto>>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		//-----------------------------
		// Domain objects
		//-----------------------------

		static auto gmodCache{
			nfx::memory::MemoryCache<VisVersion, Gmod>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto codebooksCache{
			nfx::memory::MemoryCache<VisVersion, Codebooks>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto locationsCache{
			nfx::memory::MemoryCache<VisVersion, Locations>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto gmodVersioningCache{
			nfx::memory::MemoryCache<VisVersion, internal::GmodVersioning>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		//----------------------------------------------
		// Resource loading
		//----------------------------------------------

		static std::optional<nfx::containers::StringMap<GmodVersioningDto>> loadGmodVersioningDto()
		{
			return internal::EmbeddedResource::gmodVersioning();
		}

		static std::optional<GmodDto> loadGmodDto( VisVersion version )
		{
			return internal::EmbeddedResource::gmod( VisVersionExtensions::toVersionString( version ) );
		}

		static std::optional<CodebooksDto> loadGmodcodebooksDto( VisVersion version )
		{
			return internal::EmbeddedResource::codebooks( VisVersionExtensions::toVersionString( version ) );
		}

		static std::optional<LocationsDto> loadGmodLocationsDto( VisVersion version )
		{
			return internal::EmbeddedResource::locations( VisVersionExtensions::toVersionString( version ) );
		}

		//----------------------------------------------
		// DTO access
		//----------------------------------------------

		static const nfx::containers::StringMap<GmodVersioningDto>& gmodVersioningDto( nfx::memory::MemoryCache<VisVersion, nfx::containers::StringMap<GmodVersioningDto>>& dtoCache, VisVersion version )
		{
			return dtoCache.getOrCreate(
				version,
				[]() -> nfx::containers::StringMap<GmodVersioningDto> {
					const auto dto = loadGmodVersioningDto();
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load GmodVersioningDto" };
					}

					return dto.value();
				},
				[]( nfx::memory::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		static const GmodDto& gmodDto( nfx::memory::MemoryCache<VisVersion, GmodDto>& dtoCache, VisVersion version )
		{
			return dtoCache.getOrCreate(
				version,
				[version]() -> GmodDto {
					const auto dto = loadGmodDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load GmodDto" };
					}

					return dto.value();
				},
				[]( nfx::memory::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		static const CodebooksDto& codebooksDto( nfx::memory::MemoryCache<VisVersion, CodebooksDto>& dtoCache, VisVersion version )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				throw std::invalid_argument{ "Invalid VIS version: " + std::to_string( static_cast<int>( version ) ) };
			}

			return dtoCache.getOrCreate(
				version,
				[version]() -> CodebooksDto {
					const auto dto = loadGmodcodebooksDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load CodebooksDto" };
					}

					return dto.value();
				},
				[]( nfx::memory::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		static const LocationsDto& locationsDto( nfx::memory::MemoryCache<VisVersion, LocationsDto>& dtoCache, VisVersion version )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				throw std::invalid_argument{ "Invalid VIS version: " + std::to_string( static_cast<int>( version ) ) };
			}

			return dtoCache.getOrCreate(
				version,
				[version]() -> LocationsDto {
					const auto dto = loadGmodLocationsDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load LocationsDto" };
					}

					return dto.value();
				},
				[]( nfx::memory::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		//=====================================================================
		// GMOD Versioning Manager
		//=====================================================================

		static const internal::GmodVersioning& gmodVersioning()
		{
			static constexpr VisVersion cacheKey = VisVersion::LATEST;

			return internal::gmodVersioningCache.getOrCreate(
				cacheKey,
				[]() -> internal::GmodVersioning {
					const auto& dto = internal::gmodVersioningDto( internal::gmodVersioningDtoCache, cacheKey );
					return internal::GmodVersioning{ dto };
				},
				[]( nfx::memory::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		//=====================================================================
		// Error Handling Utilities
		//=====================================================================

		[[noreturn]] static void throwInvalidVersionError( VisVersion version )
		{
			throw std::invalid_argument{ "Invalid VIS version provided: " + VisVersionExtensions::toVersionString( version ) };
		}
	}

	//=====================================================================
	// VIS singleton
	//=====================================================================

	//----------------------------------------------
	// Public interface
	//----------------------------------------------

	const Gmod& VIS::gmod( VisVersion version )
	{
		if ( !VisVersionExtensions::isValid( version ) )
		{
			throw std::invalid_argument{ "Invalid VIS version: " + VisVersionExtensions::toVersionString( version ) };
		}

		return internal::gmodCache.getOrCreate(
			version,
			[version]() -> Gmod {
				const auto& dto = internal::gmodDto( internal::gmodDtoCache, version );
				return Gmod{ version, dto };
			},
			[]( nfx::memory::CacheEntry& entry ) {
				entry.size = 1;
				entry.slidingExpiration = std::chrono::hours( 1 );
			} );
	}

	const Codebooks& VIS::codebooks( VisVersion version )
	{
		if ( !VisVersionExtensions::isValid( version ) )
		{
			throw std::invalid_argument{ "Invalid VIS version: " + VisVersionExtensions::toVersionString( version ) };
		}

		return internal::codebooksCache.getOrCreate(
			version,
			[version]() -> Codebooks {
				const auto& dto = internal::codebooksDto( internal::codebooksDtoCache, version );
				return Codebooks{ version, dto };
			},
			[]( nfx::memory::CacheEntry& entry ) {
				entry.size = 1;
				entry.slidingExpiration = std::chrono::hours( 1 );
			} );
	}

	const Locations& VIS::locations( VisVersion version )
	{
		if ( !VisVersionExtensions::isValid( version ) )
		{
			throw std::invalid_argument{ "Invalid VIS version: " + VisVersionExtensions::toVersionString( version ) };
		}

		return internal::locationsCache.getOrCreate(
			version,
			[version]() -> Locations {
				const auto& dto = internal::locationsDto( internal::locationsDtoCache, version );
				return Locations{ version, dto };
			},
			[]( nfx::memory::CacheEntry& entry ) {
				entry.size = 1;
				entry.slidingExpiration = std::chrono::hours( 1 );
			} );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	//-----------------------------
	// Bulk collection builders
	//-----------------------------

	std::unordered_map<VisVersion, Gmod> VIS::gmodsMap( const std::vector<VisVersion>& visVersions )
	{
		std::unordered_map<VisVersion, Gmod> result;
		result.reserve( visVersions.size() );
		for ( const auto& version : visVersions )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				internal::throwInvalidVersionError( version );
			}

			// Need to copy the GMOD from cache since try_emplace requires construction
			// VIS has friend access, so we can copy it here, then the map will move it
			auto gmodCopy = gmod( version );
			result.try_emplace( version, std::move( gmodCopy ) );
		}

		return result;
	}

	std::unordered_map<VisVersion, Codebooks> VIS::codebooksMap( const std::vector<VisVersion>& visVersions )
	{
		std::unordered_map<VisVersion, Codebooks> result;
		result.reserve( visVersions.size() );

		for ( const auto& version : visVersions )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				internal::throwInvalidVersionError( version );
			}

			// Need to copy the Codebooks from cache since try_emplace requires construction
			// VIS has friend access, so we can copy it here, then the map will move it
			auto codebooksCopy = codebooks( version );
			result.try_emplace( version, std::move( codebooksCopy ) );
		}

		return result;
	}

	std::unordered_map<VisVersion, Locations> VIS::locationsMap( const std::vector<VisVersion>& visVersions )
	{
		std::unordered_map<VisVersion, Locations> result;
		result.reserve( visVersions.size() );

		for ( const auto& version : visVersions )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				internal::throwInvalidVersionError( version );
			}

			// Need to copy the Locations from cache since try_emplace requires construction
			// VIS has friend access, so we can copy it here, then the map will move it
			auto locationsCopy = locations( version );
			result.try_emplace( version, std::move( locationsCopy ) );
		}

		return result;
	}

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	//-----------------------------
	// GmodNode
	//-----------------------------

	std::optional<GmodNode> VIS::convertNode( VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion )
	{
		return internal::gmodVersioning().convertNode( sourceVersion, sourceNode, targetVersion );
	}

	std::optional<GmodNode> VIS::convertNode( const GmodNode& sourceNode, VisVersion targetVersion, [[maybe_unused]] const GmodNode* sourceParent )
	{
		return convertNode( sourceNode.visVersion(), sourceNode, targetVersion );
	}

	//-----------------------------
	// GmodPath
	//-----------------------------

	std::optional<GmodPath> VIS::convertPath( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion )
	{
		return internal::gmodVersioning().convertPath( sourceVersion, sourcePath, targetVersion );
	}

	std::optional<GmodPath> VIS::convertPath( const GmodPath& sourcePath, VisVersion targetVersion )
	{
		return convertPath( sourcePath.visVersion(), sourcePath, targetVersion );
	}

	//-----------------------------
	// LocalId
	//-----------------------------

	std::optional<LocalIdBuilder> VIS::convertLocalId( const LocalIdBuilder& sourceLocalId, VisVersion targetVersion )
	{
		return internal::gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
	}

	std::optional<LocalId> VIS::convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion )
	{
		return internal::gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
	}
}
