/**
 * @file GmodVersioning.cpp
 * @brief Implementation of the GmodVersioning class for converting GMOD objects between VIS versions.
 */

#include <nfx/string/Utils.h>

#include "GmodVersioning.h"

#include "GmodVersioningNode.h"
#include "LocationSetsVisitor.h"

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/MetadataTag.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk::internal
{
	namespace
	{
		//=====================================================================
		// Path reconstruction and validation utilities
		//=====================================================================

		/**
		 * @brief Add a node to the path, ensuring valid parent-child relationships
		 * @param gmod The GMOD containing the node hierarchy
		 * @param path The current path being constructed
		 * @param node The node to add to the path
		 */
		static void addToPath( const Gmod& gmod, std::vector<GmodNode>& path, const GmodNode& node )
		{
			// Thread-local buffers for performance optimization
			static thread_local std::vector<const GmodNode*> currentParentsBuf;
			static thread_local std::vector<const GmodNode*> remainingBuf;

			if ( !path.empty() )
			{
				const GmodNode& prev = path.back();
				if ( !prev.isChild( node ) )
				{
					for ( int j = static_cast<int>( path.size() ) - 1; j >= 0; --j )
					{
						const GmodNode& parent = path[static_cast<size_t>( j )];

						currentParentsBuf.clear();
						const size_t currentParentsCount = static_cast<size_t>( j + 1 );
						currentParentsBuf.reserve( currentParentsCount );

						for ( size_t k = 0; k < currentParentsCount; ++k )
						{
							currentParentsBuf.push_back( &path[k] );
						}

						remainingBuf.clear();
						remainingBuf.reserve( 16 );

						if ( !gmod.pathExistsBetween( currentParentsBuf, node, remainingBuf ) )
						{
							bool hasOtherAssetFunction = false;
							const std::string_view parentCode = parent.code();
							for ( const GmodNode* pathNode : currentParentsBuf )
							{
								if ( pathNode->isAssetFunctionNode() && !nfx::string::equals( pathNode->code(), parentCode ) )
								{
									hasOtherAssetFunction = true;
									break;
								}
							}

							if ( !hasOtherAssetFunction )
							{
								throw std::runtime_error{ "Tried to remove last asset function node" };
							}
							path.erase( path.begin() + static_cast<std::ptrdiff_t>( j ) );
						}
						else
						{
							const auto nodeLocation = node.location();
							if ( nodeLocation.has_value() )
							{
								path.reserve( path.size() + remainingBuf.size() );

								for ( const GmodNode* n : remainingBuf )
								{
									if ( n->isIndividualizable( false, true ) )
									{
										path.emplace_back( n->tryWithLocation( nodeLocation.value() ) );
									}
									else
									{
										path.emplace_back( *n );
									}
								}
							}
							else
							{
								path.reserve( path.size() + remainingBuf.size() );
								for ( const GmodNode* n : remainingBuf )
								{
									path.emplace_back( *n );
								}
							}
							break;
						}
					}
				}
			}

			path.emplace_back( node );
		}

		/**
		 * @brief Validate source and target versions
		 */
		static void validateSourceAndTargetVersions( VisVersion sourceVersion, VisVersion targetVersion )
		{
			if ( sourceVersion == VisVersion::Unknown )
			{
				throw std::invalid_argument{ "Invalid source VIS Version: Unknown" };
			}

			if ( targetVersion == VisVersion::Unknown )
			{
				throw std::invalid_argument{ "Invalid target VIS Version: Unknown" };
			}

			if ( sourceVersion >= targetVersion )
			{
				throw std::invalid_argument{ "Source version must be earlier than target version" };
			}
		}
	}

	//=====================================================================
	// GmodVersioning class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodVersioning::GmodVersioning( const nfx::containers::StringMap<GmodVersioningDto>& dto )
	{
		m_versioningsMap.reserve( dto.size() );

		for ( const auto& [versionStr, versioningDto] : dto )
		{
			VisVersion version = VisVersionExtensions::parse( versionStr );

			m_versioningsMap.insertOrAssign( version, GmodVersioningNode( version, versioningDto.items() ) );
		}
	}

	GmodVersioning::GmodVersioning( GmodVersioning&& other ) noexcept
		: m_versioningsMap{ std::move( other.m_versioningsMap ) }
	{
	}

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	//----------------------------
	// Node
	//----------------------------

	std::optional<GmodNode> GmodVersioning::convertNode( VisVersion sourceVersion, const GmodNode& sourceNode,
		VisVersion targetVersion )
	{
		if ( nfx::string::isEmpty( sourceNode.code() ) )
		{
			return std::nullopt;
		}

		validateSourceAndTargetVersions( sourceVersion, targetVersion );

		const auto& targetGmod = VIS::instance().gmod( targetVersion );

		return convertNode( sourceVersion, sourceNode, targetVersion, targetGmod );
	}

	std::optional<GmodNode> GmodVersioning::convertNode(
		VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion, const Gmod& targetGmod )
	{
		if ( nfx::string::isEmpty( sourceNode.code() ) )
		{
			return std::nullopt;
		}

		validateSourceAndTargetVersions( sourceVersion, targetVersion );

		VisVersion nextVersion = static_cast<VisVersion>( static_cast<std::uint16_t>( sourceVersion ) + 100 );
		std::string_view nextCodeView = sourceNode.code();

		auto& versioningsMapRef = const_cast<nfx::containers::HashMap<VisVersion, GmodVersioningNode>&>( m_versioningsMap );

		while ( nextVersion <= targetVersion )
		{
			GmodVersioningNode* versioningNodePtr = nullptr;
			if ( versioningsMapRef.tryGetValue( nextVersion, versioningNodePtr ) )
			{
				const GmodNodeConversion* change = nullptr;
				if ( versioningNodePtr->tryGetCodeChanges( nextCodeView, change ) && change && change->target.has_value() )
				{
					nextCodeView = change->target.value();
				}
			}

			nextVersion = static_cast<VisVersion>( static_cast<std::uint16_t>( nextVersion ) + 100 );
		}

		// Try to find the node in the target GMOD
		GmodNode* targetNodePtr = nullptr;
		if ( !targetGmod.tryGetNode( nextCodeView, targetNodePtr ) )
		{
			return std::nullopt;
		}

		// Handle location if present
		if ( sourceNode.location().has_value() )
		{
			auto result = targetNodePtr->tryWithLocation( sourceNode.location().value() );
			if ( sourceNode.location().has_value() && result.location() != sourceNode.location() )
			{
				throw std::runtime_error( "Failed to set location" );
			}
			return result;
		}
		else
		{
			return *targetNodePtr;
		}
	}

	//----------------------------
	// Path
	//----------------------------

	std::optional<GmodPath> GmodVersioning::convertPath(
		VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion )
	{
		std::optional<GmodNode> targetEndNode = convertNode( sourceVersion, sourcePath.node(), targetVersion );
		if ( !targetEndNode.has_value() )
		{
			return std::nullopt;
		}

		auto& vis = VIS::instance();
		const auto& targetGmod = vis.gmod( targetVersion );

		if ( targetEndNode->isRoot() )
		{
			GmodNode* rootNodeInGmodPtr = nullptr;
			if ( !targetGmod.tryGetNode( targetEndNode->code(), rootNodeInGmodPtr ) || !rootNodeInGmodPtr )
			{
				throw std::runtime_error( "Failed to get root node from targetGmod during root path conversion" );
			}

			return GmodPath( {}, *rootNodeInGmodPtr );
		}

		std::vector<std::pair<const GmodNode*, GmodNode>> qualifyingNodes;

		auto fullPathEnumerator = sourcePath.fullPath();
		qualifyingNodes.reserve( 16 );

		while ( fullPathEnumerator.next() )
		{
			const auto& [depth, nodePtr] = fullPathEnumerator.current();
			if ( !nodePtr )
			{
				continue;
			}

			std::optional<GmodNode> convertedNodeOpt = convertNode( sourceVersion, *nodePtr, targetVersion, targetGmod );
			if ( !convertedNodeOpt.has_value() )
			{
				throw std::runtime_error( "Could not convert node forward" );
			}

			qualifyingNodes.emplace_back( nodePtr, std::move( *convertedNodeOpt ) );
		}

		std::vector<GmodNode> potentialParents;
		potentialParents.reserve( qualifyingNodes.size() > 0
									  ? qualifyingNodes.size() - 1
									  : 0 );

		for ( size_t i = 0; i < qualifyingNodes.size() - 1; ++i )
		{
			potentialParents.emplace_back( qualifyingNodes[i].second );
		}

		if ( GmodPath::isValid( potentialParents, *targetEndNode ) )
		{
			return GmodPath{ std::move( potentialParents ), *targetEndNode };
		}

		std::vector<GmodNode> path;
		path.reserve( 64 );

		for ( size_t i = 0; i < qualifyingNodes.size(); ++i )
		{
			const auto& qualifyingNode = qualifyingNodes[i];

			if ( i > 0 && nfx::string::equals( qualifyingNode.second.code(), qualifyingNodes[i - 1].second.code() ) )
			{
				// Continue if next qualifying node is the same as the previous
				// The same as the current qualifying node, assumes same normal assignment [IF NOT THROW EXCEPTION, uncovered case]
				if ( qualifyingNode.first->productType().has_value() &&
					 ( !qualifyingNodes[i - 1].second.productType().has_value() ||
						 !nfx::string::equals( qualifyingNode.first->productType()->code(), qualifyingNodes[i - 1].second.productType()->code() ) ) )
				{
					throw std::runtime_error{
						"Failed to convert path at node " + std::string{ qualifyingNode.second.code() } +
						". Uncovered case of merge where target node also has a new assignment" };
				}

				// Check if skipped node is individualized
				if ( qualifyingNode.second.location().has_value() )
				{
					// Find node in path
					auto it = std::find_if( path.begin(), path.end(),
						[&qualifyingNode]( const GmodNode& n ) { return nfx::string::equals( n.code(), qualifyingNode.second.code() ); } );

					if ( it != path.end() )
					{
						size_t index = static_cast<size_t>( std::distance( path.begin(), it ) );

						if ( path[index].location().has_value() &&
							 path[index].location() != qualifyingNode.second.location() )
						{
							throw std::runtime_error( "Failed to convert path at node " + std::string{ qualifyingNode.second.code() } +
													  ". Uncovered case of multiple colliding locations while converting nodes" );
						}

						if ( !path[index].isIndividualizable( false, false ) )
						{
							throw std::runtime_error( "Failed to convert path at node " + std::string{ path[index].code() } +
													  ". Uncovered case of losing individualization information" );
						}

						// Don't overwrite existing location
						if ( !path[index].location().has_value() )
						{
							path[index] = path[index].withLocation( qualifyingNode.second.location().value() );
						}
					}
				}
				continue;
			}

			const bool codeChanged = !nfx::string::equals( qualifyingNode.first->code(), qualifyingNode.second.code() );

			const std::optional<GmodNode> sourceNormalAssignment = qualifyingNode.first->productType();
			const std::optional<GmodNode> targetNormalAssignment = qualifyingNode.second.productType();

			const bool normalAssignmentChanged = sourceNormalAssignment.has_value() != targetNormalAssignment.has_value() ||
												 ( sourceNormalAssignment.has_value() && targetNormalAssignment.has_value() &&
													 !nfx::string::equals( sourceNormalAssignment->code(), targetNormalAssignment->code() ) );

			bool selectionChanged = false;

			if ( codeChanged )
			{
				addToPath( targetGmod, path, qualifyingNode.second );
			}

			else if ( normalAssignmentChanged ) // AC || AN || AD
			{
				bool wasDeleted = sourceNormalAssignment.has_value() && !targetNormalAssignment.has_value();

				if ( !codeChanged )
				{
					addToPath( targetGmod, path, qualifyingNode.second );
				}

				if ( wasDeleted )
				{
					if ( nfx::string::equals( qualifyingNode.second.code(), targetEndNode->code() ) )
					{
						if ( i + 1 < qualifyingNodes.size() )
						{
							const auto& next = qualifyingNodes[i + 1];
							if ( !nfx::string::equals( next.second.code(), qualifyingNode.second.code() ) )
							{
								throw std::runtime_error( "Normal assignment end node was deleted" );
							}
						}
					}
					continue;
				}
				else if ( !nfx::string::equals( qualifyingNode.second.code(), targetEndNode->code() ) )
				{
					if ( targetNormalAssignment.has_value() )
					{
						GmodNode targetNormalAssignmentVal = *targetNormalAssignment;
						if ( qualifyingNode.second.location().has_value() &&
							 targetNormalAssignmentVal.isIndividualizable( false, true ) )
						{
							targetNormalAssignmentVal = targetNormalAssignmentVal.tryWithLocation( *qualifyingNode.second.location() );
						}
						addToPath( targetGmod, path, targetNormalAssignmentVal );

						// AC The previous normal assignment
						if ( sourceNormalAssignment.has_value() && targetNormalAssignment.has_value() &&
							 !nfx::string::equals( sourceNormalAssignment->code(), targetNormalAssignment->code() ) )
						{
							// Sanity check that the next node is actually the old assignment
							if ( i + 1 < qualifyingNodes.size() && !nfx::string::equals( qualifyingNodes[i + 1].first->code(), sourceNormalAssignment->code() ) )
							{
								throw std::runtime_error( "Failed to convert path at node " + std::string{ qualifyingNode.second.code() } +
														  ". Expected next qualifying source node to match target normal assignment" );
							}

							// Skip next node, since that is the old assignment
							++i;
						}
					}
				}
			}

			if ( selectionChanged )
			{
				// SC || SN || SD
			}

			if ( !codeChanged && !normalAssignmentChanged )
			{
				addToPath( targetGmod, path, qualifyingNode.second );
			}

			if ( !path.empty() && nfx::string::equals( path.back().code(), targetEndNode->code() ) )
			{
				break;
			}
		}

		if ( path.empty() || path.size() < 1 )
		{
			throw std::runtime_error( "Path reconstruction resulted in an empty path" );
		}

		if ( path.size() == 1 )
		{
			return GmodPath( {}, std::move( path[0] ) );
		}

		std::vector<GmodNode> potentialParentsFromPath;
		potentialParentsFromPath.reserve( path.size() - 1 );

		for ( size_t i = 0; i < path.size() - 1; ++i )
		{
			potentialParentsFromPath.emplace_back( std::move( path[i] ) );
		}

		GmodNode targetEndNodeFromPath = std::move( path.back() );

		LocationSetsVisitor visitor;
		for ( size_t i = 0; i < potentialParentsFromPath.size() + 1; ++i )
		{
			const GmodNode& n = ( i < potentialParentsFromPath.size() )
									? potentialParentsFromPath[i]
									: targetEndNodeFromPath;

			auto set = visitor.visit( n, i, potentialParentsFromPath, targetEndNodeFromPath );
			if ( !set.has_value() )
			{
				if ( n.location().has_value() )
				{
					break;
				}
				continue;
			}

			const auto& [start, end, location] = set.value();
			if ( start == end )
				continue;

			for ( size_t j = start; j <= end; ++j )
			{
				if ( j < potentialParentsFromPath.size() && location.has_value() )
				{
					potentialParentsFromPath[j] = potentialParentsFromPath[j].withLocation( location.value() );
				}
				else if ( j >= potentialParentsFromPath.size() && location.has_value() )
				{
					targetEndNodeFromPath = targetEndNodeFromPath.withLocation( location.value() );
				}
			}
		}

		if ( !GmodPath::isValid( potentialParentsFromPath, targetEndNodeFromPath ) )
		{
			throw std::runtime_error( "Did not end up with a valid path" );
		}

		return GmodPath{ std::move( potentialParentsFromPath ), targetEndNodeFromPath };
	}

	//----------------------------
	// Local Id
	//----------------------------

	std::optional<LocalIdBuilder> GmodVersioning::convertLocalId(
		const LocalIdBuilder& sourceLocalId, VisVersion targetVersion )
	{
		if ( !sourceLocalId.visVersion().has_value() )
		{
			throw std::invalid_argument{ "Cannot convert local ID without a specific VIS version" };
		}

		LocalIdBuilder targetLocalId = LocalIdBuilder::create( targetVersion );

		if ( sourceLocalId.primaryItem().has_value() )
		{
			auto targetPrimaryItem = convertPath(
				*sourceLocalId.visVersion(), sourceLocalId.primaryItem().value(), targetVersion );

			if ( !targetPrimaryItem.has_value() )
			{
				return std::nullopt;
			}

			targetLocalId = targetLocalId.withPrimaryItem( std::move( *targetPrimaryItem ) );
		}

		if ( sourceLocalId.secondaryItem().has_value() )
		{
			auto targetSecondaryItem = convertPath(
				*sourceLocalId.visVersion(), sourceLocalId.secondaryItem().value(), targetVersion );

			if ( !targetSecondaryItem.has_value() )
			{
				return std::nullopt;
			}

			targetLocalId = targetLocalId.withSecondaryItem( std::move( *targetSecondaryItem ) );
		}

		return targetLocalId.withVerboseMode( sourceLocalId.isVerboseMode() )
			.tryWithMetadataTag( sourceLocalId.quantity() )
			.tryWithMetadataTag( sourceLocalId.content() )
			.tryWithMetadataTag( sourceLocalId.calculation() )
			.tryWithMetadataTag( sourceLocalId.state() )
			.tryWithMetadataTag( sourceLocalId.command() )
			.tryWithMetadataTag( sourceLocalId.type() )
			.tryWithMetadataTag( sourceLocalId.position() )
			.tryWithMetadataTag( sourceLocalId.detail() );
	}

	std::optional<LocalId> GmodVersioning::convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion )
	{
		auto builder = convertLocalId( sourceLocalId.builder(), targetVersion );

		return builder.has_value()
				   ? std::make_optional( builder->build() )
				   : std::nullopt;
	}
}
