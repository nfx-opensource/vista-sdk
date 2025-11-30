/**
 * @file GmodPathQuery.cpp
 * @brief Implementation of GmodPathQuery class
 */

#include "dnv/vista/sdk/GmodPathQuery.h"

#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	namespace internal
	{
		//=====================================================================
		// VIS Version Conversion Utilities
		//=====================================================================

		/**
		 * @brief Ensures the path uses the correct VIS version for query operations
		 * @param path The path to validate and potentially convert
		 * @return Path with proper VIS version for querying
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static GmodPath ensurePathVersion( const GmodPath& path )
		{
			const auto targetVersion = VIS::instance().latestVisVersion();

			if ( path.visVersion() == targetVersion )
			{
				return path;
			}

			auto convertedPath = VIS::instance().convertPath( path, targetVersion );
			if ( convertedPath.has_value() )
			{
				return std::move( convertedPath.value() );
			}

			return path;
		}

		/**
		 * @brief Ensures the node uses the correct VIS version for query operations
		 * @param node The node to validate and potentially convert
		 * @return Node with proper VIS version for querying
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static GmodNode ensureNodeVersion( const GmodNode& node )
		{
			const auto targetVersion = VIS::instance().latestVisVersion();

			if ( node.visVersion() == targetVersion )
			{
				return node;
			}

			auto convertedNode = VIS::instance().convertNode( node, targetVersion );
			if ( convertedNode.has_value() )
			{
				return std::move( convertedNode.value() );
			}

			return node;
		}
	} // namespace internal

	//=====================================================================
	// GmodPathQuery class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodPathQuery::GmodPathQuery( const GmodPath& path )
		: m_sourcePath{ path }
	{
		for ( const auto& set : m_sourcePath->individualizableSets() )
		{
			const auto nodes = set.nodes();
			if ( !nodes.empty() )
			{
				const auto& setNode = *nodes.back();
				const auto nodeCode = setNode.code();

				m_setNodes.insertOrAssign( nodeCode, setNode );

				std::vector<Location> locations;
				if ( set.location().has_value() )
				{
					locations.push_back( set.location().value() );
				}

				NodeItem item( setNode, std::move( locations ) );
				m_filter.insertOrAssign( nodeCode, std::move( item ) );
			}
		}
	}

	//----------------------------------------------
	// Build methods (Immutable fluent interface)
	//----------------------------------------------

	//----------------------------
	// Paths
	//----------------------------

	GmodPathQuery GmodPathQuery::withNode(
		std::function<const GmodNode&( const nfx::containers::FastHashMap<std::string, GmodNode>& nodes )> selector,
		bool matchAllLocations ) const
	{
		if ( !m_sourcePath.has_value() )
		{
			throw std::runtime_error{ "withNode with selector can only be used on path-based queries" };
		}

		GmodPathQuery result = *this;
		const auto& node = selector( m_setNodes );
		const auto* existingItem = result.m_filter.find( node.code() );
		if ( existingItem == nullptr )
		{
			throw std::runtime_error{ "Expected to find a filter on the node in the path" };
		}

		NodeItem updatedItem = *existingItem;
		updatedItem.setLocations( {} );
		updatedItem.setMatchAllLocations( matchAllLocations );
		result.m_filter.insertOrAssign( node.code(), std::move( updatedItem ) );

		return result;
	}

	GmodPathQuery GmodPathQuery::withNode(
		std::function<const GmodNode&( const nfx::containers::FastHashMap<std::string, GmodNode>& nodes )> selector,
		const std::vector<Location>& locations ) const
	{
		if ( !m_sourcePath.has_value() )
		{
			throw std::runtime_error{ "withNode with selector can only be used on path-based queries" };
		}

		GmodPathQuery result = *this;
		const auto& node = selector( m_setNodes );
		const auto* existingItem = result.m_filter.find( node.code() );
		if ( existingItem == nullptr )
		{
			throw std::runtime_error{ "Expected to find a filter on the node in the path" };
		}

		std::vector<Location> locationSet{ locations.begin(), locations.end() };
		NodeItem updatedItem = *existingItem;
		updatedItem.setLocations( std::move( locationSet ) );
		result.m_filter.insertOrAssign( node.code(), std::move( updatedItem ) );

		return result;
	}

	//----------------------------
	// Nodes
	//----------------------------

	GmodPathQuery GmodPathQuery::withNode( const GmodNode& node, bool matchAllLocations ) const
	{
		GmodPathQuery result = *this;
		const auto* existingItem = result.m_filter.find( node.code() );
		if ( existingItem != nullptr )
		{
			NodeItem updatedItem = *existingItem;
			updatedItem.setLocations( {} );
			updatedItem.setMatchAllLocations( matchAllLocations );
			result.m_filter.insertOrAssign( node.code(), std::move( updatedItem ) );
		}
		else
		{
			NodeItem item( node, {} );
			item.setMatchAllLocations( matchAllLocations );
			result.m_filter.insertOrAssign( node.code(), std::move( item ) );
		}

		return result;
	}

	GmodPathQuery GmodPathQuery::withNode( const GmodNode& node, const std::vector<Location>& locations ) const
	{
		GmodPathQuery result = *this;
		std::vector<Location> locationSet{ locations.begin(), locations.end() };

		const auto* existingItem = result.m_filter.find( node.code() );
		if ( existingItem != nullptr )
		{
			NodeItem updatedItem = *existingItem;
			updatedItem.setLocations( std::move( locationSet ) );
			result.m_filter.insertOrAssign( node.code(), std::move( updatedItem ) );
		}
		else
		{
			NodeItem item( node, std::move( locationSet ) );
			result.m_filter.insertOrAssign( node.code(), std::move( item ) );
		}

		return result;
	}

	//----------------------------------------------
	// Query execution
	//----------------------------------------------

	bool GmodPathQuery::match( const GmodPath* other ) const
	{
		if ( other == nullptr )
		{
			return false;
		}

		const auto target = internal::ensurePathVersion( *other );

		// Build map of target nodes and their locations
		nfx::containers::FastHashMap<std::string, std::vector<Location>> targetNodes;

		// Include all parents and the final node
		for ( const auto& parent : target.parents() )
		{
			const auto code = parent.code();
			auto* locationsPtr = targetNodes.find( code );
			if ( !locationsPtr )
			{
				targetNodes.insertOrAssign( code, std::vector<Location>{} );
				locationsPtr = targetNodes.find( code );
			}
			auto& locations = *locationsPtr;
			if ( parent.location().has_value() )
			{
				locations.push_back( parent.location().value() );
			}
		}

		const auto nodeCode = target.node().code();
		auto* nodeLocationsPtr = targetNodes.find( nodeCode );
		if ( !nodeLocationsPtr )
		{
			targetNodes.insertOrAssign( nodeCode, std::vector<Location>{} );
			nodeLocationsPtr = targetNodes.find( nodeCode );
		}
		auto& nodeLocations = *nodeLocationsPtr;
		if ( target.node().location().has_value() )
			nodeLocations.push_back( target.node().location().value() );

		// Check each filter criterion
		for ( const auto& [code, item] : m_filter )
		{
			const auto& node = internal::ensureNodeVersion( item.node() );
			const auto* potentialLocationsPtr = targetNodes.find( node.code() );
			if ( potentialLocationsPtr == nullptr )
			{
				return false;
			}

			const auto& potentialLocations = *potentialLocationsPtr;
			if ( item.matchAllLocations() )
			{
				continue;
			}

			if ( !item.locations().empty() )
			{
				if ( potentialLocations.empty() )
				{
					return false;
				}

				bool hasMatch = false;
				for ( const auto& location : potentialLocations )
				{
					if ( std::find( item.locations().begin(), item.locations().end(), location ) != item.locations().end() )
					{
						hasMatch = true;
						break;
					}
				}
				if ( !hasMatch )
				{
					return false;
				}
			}
			else
			{
				if ( !potentialLocations.empty() )
				{
					return false;
				}
			}
		}

		return true;
	}
} // namespace dnv::vista::sdk
