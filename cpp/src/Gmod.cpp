/**
 * @file Gmod.cpp
 * @brief Implementation of the Generic Product Model (GMOD) class.
 */

#include <limits>

#include <nfx/string/Utils.h>

#include "dnv/vista/sdk/Gmod.h"

#include "internal/core/GmodNode.h"
#include "internal/dto/GmodDto.h"
#include "dnv/vista/sdk/GmodPath.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Gmod class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Gmod::Gmod( VisVersion version, const GmodDto& dto )
		: m_visVersion{ version },
		  m_rootNode{ nullptr }
	{
		const auto& items = dto.items();
		std::vector<std::pair<std::string, GmodNode>> nodePairs;
		nodePairs.reserve( items.size() );

		for ( const auto& nodeDto : items )
		{
			nodePairs.emplace_back( std::string{ nodeDto.code() }, GmodNode{ version, nodeDto } );
		}

		m_nodeMap = nfx::containers::ChdHashMap<GmodNode>( std::move( nodePairs ) );

		for ( const auto& relation : dto.relations() )
		{
			if ( relation.size() >= 2 )
			{
				/*
				 * Each relation defines a parent-child relationship and must contain at least 2 elements:
				 * relation[0] = parent node code (e.g., "VE", "400", "410")
				 * relation[1] = child node code (e.g., "400", "410", "411")
				 * We need both parent and child codes to establish the bidirectional relationship.
				 * Relations with < 2 elements are malformed and would cause array access errors.
				 */
				auto& parentNode = const_cast<GmodNode&>( m_nodeMap[relation[0]] );
				auto& childNode = const_cast<GmodNode&>( m_nodeMap[relation[1]] );

				parentNode.addChild( &childNode );
				childNode.addParent( &parentNode );
			}
		}

		for ( auto& [key, node] : m_nodeMap )
		{
			const_cast<GmodNode&>( node ).trim();
		}

		m_rootNode = const_cast<GmodNode*>( &m_nodeMap["VE"] );
	}

	Gmod::Gmod( VisVersion version, const std::unordered_map<std::string, GmodNode>& nodeMap )
		: m_visVersion{ version }
	{
		std::vector<std::pair<std::string, GmodNode>> pairs;
		pairs.reserve( nodeMap.size() );
		for ( const auto& [code, node] : nodeMap )
		{
			pairs.emplace_back( code, node );
		}
		m_nodeMap = nfx::containers::ChdHashMap<GmodNode>( std::move( pairs ) );

		for ( auto& [key, node] : m_nodeMap )
		{
			const_cast<GmodNode&>( node ).trim();
		}

		m_rootNode = const_cast<GmodNode*>( &m_nodeMap["VE"] );
	}

	Gmod::Gmod( const Gmod& other )
		: m_visVersion{ other.m_visVersion },
		  m_nodeMap{ other.m_nodeMap }
	{
		m_rootNode = const_cast<GmodNode*>( &m_nodeMap["VE"] );
	}

	Gmod::Gmod( Gmod&& other ) noexcept
		: m_visVersion{ other.m_visVersion },
		  m_nodeMap{ std::move( other.m_nodeMap ) }
	{
		if ( other.m_rootNode != nullptr )
		{
			m_rootNode = &m_nodeMap["VE"];
			other.m_rootNode = nullptr;
		}
		else
		{
			m_rootNode = nullptr;
		}
	}

	//----------------------------------------------
	// Path parsing & navigation
	//----------------------------------------------

	GmodPath Gmod::parsePath( std::string_view item ) const
	{
		return GmodPath::parse( item, m_visVersion );
	}

	bool Gmod::tryParsePath( std::string_view item, std::optional<GmodPath>& path ) const noexcept
	{
		return GmodPath::tryParse( item, m_visVersion, path );
	}

	GmodPath Gmod::parseFromFullPath( std::string_view item ) const
	{
		return GmodPath::parseFullPath( item, m_visVersion );
	}

	bool Gmod::tryParseFromFullPath( std::string_view item, std::optional<GmodPath>& path ) const noexcept
	{
		return GmodPath::tryParseFullPath( item, m_visVersion, path );
	}

	//----------------------------------------------
	// Static state inspection methods
	//----------------------------------------------

	bool Gmod::isPotentialParent( std::string_view type ) noexcept
	{
		/**
		 * Length 4: "Leaf" - internal::gmodnode::TYPE_LEAF
		 *   - Represents terminal nodes in the GMOD hierarchy
		 *   - Cannot have child nodes
		 *
		 * Length 5: "Group" - internal::gmodnode::TYPE_GROUP
		 *   - Represents container nodes that can hold other nodes
		 *   - Used for organizational grouping in the hierarchy
		 *
		 * Length 9: "Selection" - internal::gmodnode::TYPE_SELECTION
		 *   - Represents selection nodes for product configurations
		 *   - Can contain multiple selectable options
		 */

		if ( nfx::string::hasExactLength( type, 4 ) )
		{
			return nfx::string::equals( type, internal::gmodnode::TYPE_LEAF );
		}
		else if ( nfx::string::hasExactLength( type, 5 ) )
		{
			return nfx::string::equals( type, internal::gmodnode::TYPE_GROUP );
		}
		else if ( nfx::string::hasExactLength( type, 9 ) )
		{
			return nfx::string::equals( type, internal::gmodnode::TYPE_SELECTION );
		}

		return false;
	}

	bool Gmod::isLeafNode( const GmodNodeMetadata& metadata ) noexcept
	{
		const auto& fullType = metadata.fullType();
		return nfx::string::equals( fullType, internal::gmodnode::FULLTYPE_ASSET_FUNCTION_LEAF ) ||
			   nfx::string::equals( fullType, internal::gmodnode::FULLTYPE_PRODUCT_FUNCTION_LEAF );
	}

	bool Gmod::isFunctionNode( const GmodNodeMetadata& metadata ) noexcept
	{
		const auto& category = metadata.category();

		return !nfx::string::equals( category, internal::gmodnode::CATEGORY_PRODUCT ) &&
			   !nfx::string::equals( category, internal::gmodnode::CATEGORY_ASSET );
	}

	bool Gmod::isProductSelection( const GmodNodeMetadata& metadata ) noexcept
	{
		return nfx::string::equals( metadata.category(), internal::gmodnode::CATEGORY_PRODUCT ) &&
			   nfx::string::equals( metadata.type(), internal::gmodnode::TYPE_SELECTION );
	}

	bool Gmod::isProductType( const GmodNodeMetadata& metadata ) noexcept
	{
		return nfx::string::equals( metadata.category(), internal::gmodnode::CATEGORY_PRODUCT ) &&
			   nfx::string::equals( metadata.type(), internal::gmodnode::TYPE_TYPE );
	}

	bool Gmod::isAsset( const GmodNodeMetadata& metadata ) noexcept
	{
		return nfx::string::equals( metadata.category(), internal::gmodnode::CATEGORY_ASSET );
	}

	bool Gmod::isAssetFunctionNode( const GmodNodeMetadata& metadata ) noexcept
	{
		return nfx::string::equals( metadata.category(), internal::gmodnode::ASSET_FUNCTION );
	}

	bool Gmod::isProductTypeAssignment( const GmodNode* parent, const GmodNode* child ) noexcept
	{
		if ( !parent || !child )
		{
			return false;
		}

		const auto& parentCategory = parent->metadata().category();
		const auto& childCategory = child->metadata().category();
		const auto& childType = child->metadata().type();

		if ( !nfx::string::contains( parentCategory, internal::gmodnode::CATEGORY_FUNCTION ) )
		{
			return false;
		}
		if ( !nfx::string::equals( childCategory, internal::gmodnode::CATEGORY_PRODUCT ) ||
			 !nfx::string::equals( childType, internal::gmodnode::TYPE_TYPE ) )
		{
			return false;
		}

		return true;
	}

	bool Gmod::isProductSelectionAssignment( const GmodNode* parent, const GmodNode* child ) noexcept
	{
		if ( !parent || !child )
		{
			return false;
		}

		const auto& parentCategory = parent->metadata().category();
		const auto& childCategory = child->metadata().category();
		const auto& childType = child->metadata().type();

		if ( !nfx::string::contains( parentCategory, internal::gmodnode::CATEGORY_FUNCTION ) )
		{
			return false;
		}
		if ( !nfx::string::equals( childCategory, internal::gmodnode::CATEGORY_PRODUCT ) ||
			 !nfx::string::equals( childType, internal::gmodnode::TYPE_SELECTION ) )
		{
			return false;
		}

		return true;
	}

	//----------------------------------------------
	// Traversal methods
	//----------------------------------------------

	bool Gmod::pathExistsBetween( const std::vector<const GmodNode*>& fromPath, const GmodNode& to,
		std::vector<const GmodNode*>& remainingParents ) const
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

		auto handler = +[]( PathExistsState& s, const std::vector<const GmodNode*>& parents,
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

		traverse( state, startNode, handler );

		return state.found;
	}
}
