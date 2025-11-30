/**
 * @file VIS.cpp
 * @brief Implementation of Vessel Information Structure (VIS) interface
 */

#include <nfx/Containers.h>
#include <nfx/cache/LruCache.h>

#include "dnv/vista/sdk/VIS.h"

#include "internal/core/EmbeddedResource.h"
#include "internal/core/GmodVersioning.h"
#include "internal/dto/CodebooksDto.h"
#include "internal/dto/GmodDto.h"
#include "internal/dto/GmodVersioningDto.h"
#include "internal/dto/LocationsDto.h"
#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/Locations.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	namespace internal::vis
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
			nfx::cache::LruCache<VisVersion, GmodDto>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto codebooksDtoCache{
			nfx::cache::LruCache<VisVersion, CodebooksDto>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto locationsDtoCache{
			nfx::cache::LruCache<VisVersion, LocationsDto>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto gmodVersioningDtoCache{
			nfx::cache::LruCache<VisVersion, nfx::containers::FastHashMap<std::string, GmodVersioningDto>>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		//-----------------------------
		// Domain objects
		//-----------------------------

		static auto gmodCache{
			nfx::cache::LruCache<VisVersion, Gmod>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto codebooksCache{
			nfx::cache::LruCache<VisVersion, Codebooks>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto locationsCache{
			nfx::cache::LruCache<VisVersion, Locations>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		static auto gmodVersioningCache{
			nfx::cache::LruCache<VisVersion, GmodVersioning>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		//----------------------------------------------
		// Resource loading
		//----------------------------------------------

		static std::optional<nfx::containers::FastHashMap<std::string, GmodVersioningDto>> loadGmodVersioningDto()
		{
			return EmbeddedResource::gmodVersioning();
		}

		static std::optional<GmodDto> loadGmodDto( VisVersion version )
		{
			return EmbeddedResource::gmod( VisVersionExtensions::toVersionString( version ) );
		}

		static std::optional<CodebooksDto> loadGmodcodebooksDto( VisVersion version )
		{
			return EmbeddedResource::codebooks( VisVersionExtensions::toVersionString( version ) );
		}

		static std::optional<LocationsDto> loadGmodLocationsDto( VisVersion version )
		{
			return EmbeddedResource::locations( VisVersionExtensions::toVersionString( version ) );
		}

		//----------------------------------------------
		// DTO access
		//----------------------------------------------

		static const nfx::containers::FastHashMap<std::string, GmodVersioningDto>& gmodVersioningDto( nfx::cache::LruCache<VisVersion, nfx::containers::FastHashMap<std::string, GmodVersioningDto>>& dtoCache, VisVersion version )
		{
			return *dtoCache.get(
				version,
				[]() -> nfx::containers::FastHashMap<std::string, GmodVersioningDto> {
					const auto dto = loadGmodVersioningDto();
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load GmodVersioningDto" };
					}

					return dto.value();
				},
				[]( nfx::cache::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		static const GmodDto& gmodDto( nfx::cache::LruCache<VisVersion, GmodDto>& dtoCache, VisVersion version )
		{
			return *dtoCache.get(
				version,
				[version]() -> GmodDto {
					const auto dto = loadGmodDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load GmodDto" };
					}

					return dto.value();
				},
				[]( nfx::cache::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		static const CodebooksDto& codebooksDto( nfx::cache::LruCache<VisVersion, CodebooksDto>& dtoCache, VisVersion version )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				throw std::invalid_argument{ "Invalid VIS version: " + std::to_string( static_cast<int>( version ) ) };
			}

			return *dtoCache.get(
				version,
				[version]() -> CodebooksDto {
					const auto dto = loadGmodcodebooksDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load CodebooksDto" };
					}

					return dto.value();
				},
				[]( nfx::cache::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		static const LocationsDto& locationsDto( nfx::cache::LruCache<VisVersion, LocationsDto>& dtoCache, VisVersion version )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				throw std::invalid_argument{ "Invalid VIS version: " + std::to_string( static_cast<int>( version ) ) };
			}

			return *dtoCache.get(
				version,
				[version]() -> LocationsDto {
					const auto dto = loadGmodLocationsDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load LocationsDto" };
					}

					return dto.value();
				},
				[]( nfx::cache::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		//----------------------------------------------
		// GMOD versioning
		//----------------------------------------------

		static GmodVersioning& gmodVersioning()
		{
			static constexpr VisVersion cacheKey = VisVersion::Latest;

			return *gmodVersioningCache.get(
				cacheKey,
				[]() -> internal::GmodVersioning {
					const auto& dto = gmodVersioningDto( gmodVersioningDtoCache, cacheKey );

					return GmodVersioning{ dto };
				},
				[]( nfx::cache::CacheEntry& entry ) {
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
	} // namespace internal::vis

	//=====================================================================
	// VIS singleton
	//=====================================================================

	//----------------------------------------------
	// Public interface
	//----------------------------------------------

	const Gmod& VIS::gmod( VisVersion version ) const
	{
		if ( !VisVersionExtensions::isValid( version ) )
		{
			throw std::invalid_argument{ "Invalid VIS version: " + VisVersionExtensions::toVersionString( version ) };
		}

		return *internal::vis::gmodCache.get(
			version,
			[version]() -> Gmod {
				const auto& dto = internal::vis::gmodDto( internal::vis::gmodDtoCache, version );
				return Gmod{ version, dto };
			},
			[]( nfx::cache::CacheEntry& entry ) {
				entry.size = 1;
				entry.slidingExpiration = std::chrono::hours( 1 );
			} );
	}

	const Codebooks& VIS::codebooks( VisVersion version ) const
	{
		if ( !VisVersionExtensions::isValid( version ) )
		{
			throw std::invalid_argument{ "Invalid VIS version: " + VisVersionExtensions::toVersionString( version ) };
		}

		return *internal::vis::codebooksCache.get(
			version,
			[version]() -> Codebooks {
				const auto& dto = internal::vis::codebooksDto( internal::vis::codebooksDtoCache, version );
				return Codebooks{ version, dto };
			},
			[]( nfx::cache::CacheEntry& entry ) {
				entry.size = 1;
				entry.slidingExpiration = std::chrono::hours( 1 );
			} );
	}

	const Locations& VIS::locations( VisVersion version ) const
	{
		if ( !VisVersionExtensions::isValid( version ) )
		{
			throw std::invalid_argument{ "Invalid VIS version: " + VisVersionExtensions::toVersionString( version ) };
		}

		return *internal::vis::locationsCache.get(
			version,
			[version]() -> Locations {
				const auto& dto = internal::vis::locationsDto( internal::vis::locationsDtoCache, version );
				return Locations{ version, dto };
			},
			[]( nfx::cache::CacheEntry& entry ) {
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
				internal::vis::throwInvalidVersionError( version );
			}

			/* Need to copy the GMOD from cache since try_emplace requires construction
			 * VIS has friend access, so we can copy it here, then the map will move it
			 */
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
				internal::vis::throwInvalidVersionError( version );
			}

			/* Need to copy the Codebooks from cache since try_emplace requires construction
			 * VIS has friend access, so we can copy it here, then the map will move it
			 */
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
				internal::vis::throwInvalidVersionError( version );
			}

			/* Need to copy the Locations from cache since try_emplace requires construction
			 * VIS has friend access, so we can copy it here, then the map will move it
			 */
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

	std::optional<GmodNode> VIS::convertNode( VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const
	{
		return internal::vis::gmodVersioning().convertNode( sourceVersion, sourceNode, targetVersion );
	}

	std::optional<GmodNode> VIS::convertNode( const GmodNode& sourceNode, VisVersion targetVersion, [[maybe_unused]] const GmodNode* sourceParent ) const
	{
		return convertNode( sourceNode.visVersion(), sourceNode, targetVersion );
	}

	//-----------------------------
	// GmodPath
	//-----------------------------

	std::optional<GmodPath> VIS::convertPath( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const
	{
		return internal::vis::gmodVersioning().convertPath( sourceVersion, sourcePath, targetVersion );
	}

	std::optional<GmodPath> VIS::convertPath( const GmodPath& sourcePath, VisVersion targetVersion ) const
	{
		return convertPath( sourcePath.visVersion(), sourcePath, targetVersion );
	}

	//-----------------------------
	// LocalId
	//-----------------------------

	std::optional<LocalIdBuilder> VIS::convertLocalId( const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const
	{
		return internal::vis::gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
	}

	std::optional<LocalId> VIS::convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion ) const
	{
		return internal::vis::gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
	}
} // namespace dnv::vista::sdk
