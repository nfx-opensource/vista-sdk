/**
 * @file VISVersion.inl
 * @brief Inline implementations for performance-critical VISVersion operations
 */

#include <stdexcept>

#include <nfx/Containers.h>
#include <nfx/string/StringBuilder.h>

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Static lookup data
		//=====================================================================

		inline const std::vector<VisVersion>& allVersionsImpl()
		{
			static const std::vector<VisVersion> versions = []() {
				std::vector<VisVersion> result;
				for ( VisVersion v = VisVersion::v3_4a; v <= VisVersion::Latest; v = v + 1 )
				{
					result.push_back( v );
				}
				return result;
			}();
			return versions;
		}

		inline const nfx::containers::FastHashMap<std::string, VisVersion>& versionMapImpl()
		{
			static const auto versionMap = []() {
				nfx::containers::FastHashMap<std::string, VisVersion> map;
				map.reserve( 12 );
				map.insertOrAssign( "3-4a", VisVersion::v3_4a );
				map.insertOrAssign( "3-5a", VisVersion::v3_5a );
				map.insertOrAssign( "3-6a", VisVersion::v3_6a );
				map.insertOrAssign( "3-7a", VisVersion::v3_7a );
				map.insertOrAssign( "3-8a", VisVersion::v3_8a );
				map.insertOrAssign( "3-9a", VisVersion::v3_9a );
				map.insertOrAssign( "vis-3-4a", VisVersion::v3_4a );
				map.insertOrAssign( "vis-3-5a", VisVersion::v3_5a );
				map.insertOrAssign( "vis-3-6a", VisVersion::v3_6a );
				map.insertOrAssign( "vis-3-7a", VisVersion::v3_7a );
				map.insertOrAssign( "vis-3-8a", VisVersion::v3_8a );
				map.insertOrAssign( "vis-3-9a", VisVersion::v3_9a );
				return map;
			}();

			return versionMap;
		}
	} // namespace

	inline VisVersion operator++( VisVersion& version )
	{
		version = static_cast<VisVersion>( static_cast<int>( version ) + 100 );

		return version;
	}

	inline VisVersion operator++( VisVersion& version, int )
	{
		VisVersion old = version;
		++version;

		return old;
	}

	inline VisVersion operator+( VisVersion version, int increment )
	{
		if ( increment == 1 )
		{
			return static_cast<VisVersion>( static_cast<int>( version ) + 100 );
		}

		return static_cast<VisVersion>( static_cast<int>( version ) + increment * 100 );
	}

	inline VisVersion operator-( VisVersion version, int decrement )
	{
		if ( decrement == 1 )
		{
			return static_cast<VisVersion>( static_cast<int>( version ) - 100 );
		}

		return static_cast<VisVersion>( static_cast<int>( version ) - decrement * 100 );
	}

	inline bool operator<=( VisVersion lhs, VisVersion rhs )
	{
		return static_cast<int>( lhs ) <= static_cast<int>( rhs );
	}

	inline int operator-( VisVersion lhs, VisVersion rhs )
	{
		return ( static_cast<int>( lhs ) - static_cast<int>( rhs ) ) / 100;
	}

	inline bool VisVersionExtensions::isValid( VisVersion version )
	{
		return version >= VisVersion::v3_4a && version <= VisVersion::Latest;
	}

	inline const std::vector<VisVersion>& VisVersionExtensions::allVersions()
	{
		return allVersionsImpl();
	}

	inline VisVersion VisVersionExtensions::latestVersion()
	{
		return VisVersion::Latest;
	}

	inline std::string VisVersionExtensions::toVersionString( VisVersion version )
	{
		switch ( version )
		{
			case VisVersion::v3_4a:
			{
				return "vis-3-4a";
			}
			case VisVersion::v3_5a:
			{
				return "vis-3-5a";
			}
			case VisVersion::v3_6a:
			{
				return "vis-3-6a";
			}
			case VisVersion::v3_7a:
			{
				return "vis-3-7a";
			}
			case VisVersion::v3_8a:
			{
				return "vis-3-8a";
			}
			case VisVersion::v3_9a:
			{
				return "vis-3-9a";
			}
			case VisVersion::Unknown:
			{
				return "Unknown";
			}
			default:
			{
				throw std::invalid_argument{ "Invalid VIS version: " + std::to_string( static_cast<int>( version ) ) };
			}
		}
	}

	inline bool VisVersionExtensions::tryParse( std::string_view versionString, VisVersion& version )
	{
		const auto& versionMap = versionMapImpl();
		const auto* ptr = versionMap.find( versionString );
		if ( ptr != nullptr )
		{
			version = *ptr;

			return true;
		}
		return false;
	}

	inline VisVersion VisVersionExtensions::parse( std::string_view versionString )
	{
		VisVersion result;
		if ( !tryParse( versionString, result ) )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.create();
			builder.append( "Invalid VIS version string: " );
			builder.append( versionString );
			throw std::invalid_argument{ lease.toString() };
		}

		return result;
	}
} // namespace dnv::vista::sdk
