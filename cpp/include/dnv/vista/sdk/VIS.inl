/**
 * @file VIS.inl
 * @brief Inline implementations for performance-critical VIS operations
 */

#include "CodebooksDto.h"
#include "GmodDto.h"
#include "LocationsDto.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// VIS singleton
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline VIS::VIS()
	{
	}

	//----------------------------------------------
	// Singleton access
	//----------------------------------------------

	inline VIS& VIS::instance()
	{
		static VIS instance;

		return instance;
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	//-----------------------------
	// VisVersion
	//-----------------------------

	inline VisVersion VIS::latestVisVersion() const noexcept
	{
		return VisVersion::LATEST;
	}

	inline const std::vector<VisVersion>& VIS::visVersions() const noexcept
	{
		return VisVersionExtensions::allVersions();
	}

	//-----------------------------
	// Cached objects
	//-----------------------------

	inline const Gmod& VIS::gmod( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument{ "Invalid VIS version: " + std::to_string( static_cast<int>( visVersion ) ) };
		}

		return gmodsCache().getOrCreate( visVersion, [visVersion]() {
			const auto& dto = instance().gmodDto( visVersion );

			return Gmod{ visVersion, dto };
		} );
	}

	inline const Codebooks& VIS::codebooks( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument{ "Invalid VIS version: " + std::to_string( static_cast<int>( visVersion ) ) };
		}

		return codebooksCache().getOrCreate( visVersion, [visVersion]() {
			const auto& dto = instance().codebooksDto( visVersion );

			return Codebooks{ visVersion, dto };
		} );
	}

	inline const Locations& VIS::locations( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument{ "Invalid VIS version: " + std::to_string( static_cast<int>( visVersion ) ) };
		}

		return locationsCache().getOrCreate(
			visVersion, [visVersion]() {
				const auto& dto = instance().locationsDto( visVersion );
				return Locations{ visVersion, dto };
			} );
	}

	//----------------------------------------------
	// ISO string validation
	//----------------------------------------------

	template <typename StringLike>
	inline bool VIS::matchISOLocalIdString( const StringLike& value ) noexcept
	{
		std::string_view sv = value;
		for ( char c : sv )
		{
			if ( c == '/' )
			{
				continue;
			}
			if ( !isISOString( c ) )
			{
				return false;
			}
		}

		return true;
	}

	template <typename StringLike>
	inline bool VIS::isISOString( const StringLike& value ) noexcept
	{
		std::string_view sv = value;
		for ( char c : sv )
		{
			if ( !isISOString( c ) )
			{
				return false;
			}
		}

		return true;
	}

	template <typename StringLike>
	inline bool VIS::isISOLocalIdString( const StringLike& value ) noexcept
	{
		return matchISOLocalIdString( value );
	}

	inline bool VIS::isISOString( char c ) noexcept
	{
		return matchAsciiDecimal( static_cast<int>( c ) );
	}

	inline constexpr bool VIS::matchAsciiDecimal( int code ) noexcept
	{
		return ( code >= 48 && code <= 57 )		// Numbers:     0-9
			   || ( code >= 65 && code <= 90 )	// Uppercase:   A-Z
			   || ( code >= 97 && code <= 122 ) // Lowercase:   a-z
			   || ( code == 45					// Hyphen:      -
					  || code == 46				// Period:      .
					  || code == 95				// Underscore:  _
					  || code == 126 );			// Tilde:       ~
	}
}
