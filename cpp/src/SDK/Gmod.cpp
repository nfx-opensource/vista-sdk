/**
 * @file Gmod.cpp
 * @brief Implementation of Gmod class methods
 * @details Provides constructors and static helper methods for Gmod tree management
 */

#include "dnv/vista/sdk/Gmod.h"

#include "dto/GmodDto.h"

#include <nfx/StringUtils.h>

#include <algorithm>
#include <stdexcept>

namespace dnv::vista::sdk
{
	Gmod::Gmod( VisVersion version, const GmodDto& dto )
		: m_visVersion{ version },
		  m_rootNode{ nullptr }
	{
		const auto& items = dto.items;
		std::vector<std::pair<std::string, GmodNode>> nodePairs;
		nodePairs.reserve( items.size() );

		for ( const auto& nodeDto : items )
		{
			nodePairs.emplace_back( std::string{ nodeDto.code }, GmodNode{ version, nodeDto } );
		}

		m_nodeMap = ChdDictionary<std::string, GmodNode>( std::move( nodePairs ) );

		for ( const auto& relation : dto.relations )
		{
			/*
			 * Each relation defines a parent-child relationship and must contain at least 2 elements:
			 * relation[0] = parent node code (e.g., "VE", "400", "410")
			 * relation[1] = child node code (e.g., "400", "410", "411")
			 * We need both parent and child codes to establish the bidirectional relationship.
			 * Relations with < 2 elements are malformed and would cause array access errors.
			 */
			const auto* parentNodePtr = m_nodeMap.find( relation[0] );
			if ( !parentNodePtr )
			{
				throw std::runtime_error{ "Parent node not found in Gmod" };
			}
			const auto* childNodePtr = m_nodeMap.find( relation[1] );
			if ( !childNodePtr )
			{
				throw std::runtime_error{ "Child node not found in Gmod" };
			}
			auto& parentNode = const_cast<GmodNode&>( *parentNodePtr );
			auto& childNode = const_cast<GmodNode&>( *childNodePtr );
			parentNode.addChild( &childNode );
			childNode.addParent( &parentNode );
		}

		for ( auto& [key, node] : m_nodeMap )
		{
			const_cast<GmodNode&>( node ).trim();
		}

		const auto* rootPtr = m_nodeMap.find( "VE" );
		if ( !rootPtr )
		{
			throw std::runtime_error{ "Root node 'VE' not found in Gmod" };
		}
		m_rootNode = const_cast<GmodNode*>( rootPtr );
	}

	bool Gmod::traverse( TraverseHandler handler, TraversalOptions options ) const
	{
		return traverse( *m_rootNode, handler, options );
	}

	bool Gmod::traverse( const GmodNode& rootNode, TraverseHandler handler, TraversalOptions options ) const
	{
		struct DummyState
		{
		};

		DummyState state;

		TraverseHandlerWithState<DummyState> wrappedHandler =
			[&handler]( DummyState&, const std::vector<const GmodNode*>& parents, const GmodNode& node )
			-> TraversalHandlerResult {
			return handler( parents, node );
		};

		return traverse( state, rootNode, wrappedHandler, options );
	}

	bool Gmod::pathExistsBetween( const std::vector<const GmodNode*>& fromPath, const GmodNode& to, std::vector<const GmodNode*>& remainingParents ) const
	{
		remainingParents.clear();

		const GmodNode* lastAssetFunction = nullptr;
		size_t assetFunctionIndex = std::numeric_limits<size_t>::max();
		for ( size_t i = fromPath.size(); i > 0; --i )
		{
			size_t idx = i - 1;
			if ( fromPath[idx]->isAssetFunctionNode() )
			{
				lastAssetFunction = fromPath[idx];
				assetFunctionIndex = idx;
				break;
			}
		}

		const GmodNode& startNode = lastAssetFunction
										? *lastAssetFunction
										: rootNode();

		struct PathExistsState
		{
			const GmodNode& targetNode;
			const std::vector<const GmodNode*>& fromPath;
			std::vector<const GmodNode*>& remainingParents;
			size_t assetFunctionIndex;
			bool found = false;

			PathExistsState( const GmodNode& target, const std::vector<const GmodNode*>& from,
				std::vector<const GmodNode*>& remaining, size_t afIndex )
				: targetNode{ target },
				  fromPath{ from },
				  remainingParents{ remaining },
				  assetFunctionIndex{ afIndex }
			{
			}

			PathExistsState( const PathExistsState& ) = delete;
			PathExistsState( PathExistsState&& ) = delete;
			PathExistsState& operator=( const PathExistsState& ) = delete;
			PathExistsState& operator=( PathExistsState&& ) = delete;
		};

		PathExistsState state( to, fromPath, remainingParents, assetFunctionIndex );

		auto handler = [this]( PathExistsState& s, const std::vector<const GmodNode*>& parents,
						   const GmodNode& node ) -> TraversalHandlerResult {
			if ( !nfx::string::equals( node.code(), s.targetNode.code() ) )
			{
				return TraversalHandlerResult::Continue;
			}

			std::vector<const GmodNode*> completePath;
			completePath.reserve( parents.size() );

			for ( const GmodNode* parent : parents )
			{
				if ( !parent->isRoot() )
				{
					completePath.push_back( parent );
				}
			}

			size_t startIndex = 0;

			if ( s.assetFunctionIndex != std::numeric_limits<size_t>::max() )
			{
				startIndex = s.assetFunctionIndex;
			}

			size_t requiredNodes = s.fromPath.size() - startIndex;
			if ( completePath.size() < requiredNodes )
			{
				return TraversalHandlerResult::Continue;
			}

			bool match = true;
			for ( size_t i = 0; i < requiredNodes; ++i )
			{
				size_t fromPathIdx = startIndex + i;
				if ( fromPathIdx >= s.fromPath.size() || i >= completePath.size() )
				{
					match = false;
					break;
				}
				if ( !nfx::string::equals( completePath[i]->code(), s.fromPath[fromPathIdx]->code() ) )
				{
					match = false;
					break;
				}
			}

			if ( match )
			{
				for ( size_t i = requiredNodes; i < completePath.size(); ++i )
				{
					s.remainingParents.push_back( completePath[i] );
				}
				s.found = true;
				return TraversalHandlerResult::Stop;
			}

			return TraversalHandlerResult::Continue;
		};

		auto wrappedHandler = [&state, &handler]( const std::vector<const GmodNode*>& parents, const GmodNode& node ) -> TraversalHandlerResult {
			return handler( state, parents, node );
		};

		traverse( startNode, wrappedHandler, TraversalOptions{} );

		return state.found;
	}
} // namespace dnv::vista::sdk
