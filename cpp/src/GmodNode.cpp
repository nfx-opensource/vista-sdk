/**
 * @file GmodNode.cpp
 * @brief Implementation of the GmodNode and GmodNodeMetadata classes
 */

#include "internal/constants/GmodNode.h"

#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	namespace internal
	{
		//=====================================================================
		// Relationship utility
		//=====================================================================

		static constexpr size_t estimateChildrenCount( std::string_view category, std::string_view type ) noexcept
		{
			if ( category == internal::constants::gmodnode::CATEGORY_PRODUCT && type == internal::constants::gmodnode::TYPE_TYPE )
			{
				return 0;
			}
			if ( nfx::string::contains( category, internal::constants::gmodnode::CATEGORY_FUNCTION ) )
			{
				return 16;
			}
			if ( category == internal::constants::gmodnode::CATEGORY_ASSET )
			{
				return 4;
			}
			return 8;
		}

		static constexpr size_t estimateParentsCount( std::string_view category, std::string_view type ) noexcept
		{
			if ( category == internal::constants::gmodnode::CATEGORY_PRODUCT && type == internal::constants::gmodnode::TYPE_TYPE )
			{
				return 1;
			}
			if ( nfx::string::contains( category, internal::constants::gmodnode::CATEGORY_FUNCTION ) )
			{
				return 2;
			}
			if ( category == internal::constants::gmodnode::CATEGORY_ASSET )
			{
				return 1;
			}

			return 1;
		}
	}

	//=====================================================================
	// GmodNode class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodNode::GmodNode( VisVersion version, const GmodNodeDto& dto ) noexcept
		: m_code{ dto.code() },
		  m_location{ std::nullopt },
		  m_visVersion{ version },
		  m_metadata{
			  dto.category(),
			  dto.type(), dto.name(),
			  dto.commonName(),
			  dto.definition(),
			  dto.commonDefinition(),
			  dto.installSubstructure(),
			  dto.normalAssignmentNames().has_value() ? *dto.normalAssignmentNames()
													  : nfx::containers::StringMap<std::string>() },
		  m_children{},
		  m_parents{},
		  m_childrenSet{}
	{
		size_t expectedChildren = internal::estimateChildrenCount( dto.category(), dto.type() );
		size_t expectedParents = internal::estimateParentsCount( dto.category(), dto.type() );

		m_children.reserve( expectedChildren );
		m_parents.reserve( expectedParents );
		m_childrenSet.reserve( expectedChildren );
	}

	//----------------------------------------------
	// Node location methods
	//----------------------------------------------

	GmodNode GmodNode::withoutLocation() const
	{
		if ( !m_location.has_value() )
		{
			return *this;
		}

		GmodNode result = *this;
		result.m_location = std::nullopt;

		return result;
	}

	GmodNode GmodNode::withLocation( std::string_view locationStr ) const
	{
		Locations locations = VIS::instance().locations( m_visVersion );
		Location location = locations.parse( locationStr );

		return withLocation( location );
	}

	GmodNode GmodNode::tryWithLocation( std::string_view locationStr ) const
	{
		Locations locations = VIS::instance().locations( m_visVersion );
		Location parsedLocation;

		if ( !locations.tryParse( locationStr, parsedLocation ) )
		{
			return *this;
		}

		return withLocation( parsedLocation );
	}

	GmodNode GmodNode::tryWithLocation( std::string_view locationStr, ParsingErrors& errors ) const
	{
		auto locations = VIS::instance().locations( m_visVersion );

		Location location;
		if ( !locations.tryParse( locationStr, location, errors ) )
		{
			return *this;
		}

		return withLocation( location );
	}

	GmodNode GmodNode::tryWithLocation( const std::optional<Location>& location ) const
	{
		if ( !location.has_value() )
		{
			return *this;
		}

		if ( m_location.has_value() && m_location.value() == location.value() )
		{
			return *this;
		}

		return withLocation( location.value() );
	}

	//----------------------------------------------
	// Relationship accessors
	//----------------------------------------------

	std::optional<GmodNode> GmodNode::productType() const noexcept
	{
		if ( m_children.size() != 1 )
		{
			return std::nullopt;
		}

		if ( !nfx::string::contains( m_metadata.category(), internal::constants::gmodnode::CATEGORY_FUNCTION ) )
		{
			return std::nullopt;
		}

		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return std::nullopt;
		}

		if ( child->m_metadata.category() != internal::constants::gmodnode::CATEGORY_PRODUCT )
		{
			return std::nullopt;
		}

		if ( child->m_metadata.type() != internal::constants::gmodnode::TYPE_TYPE )
		{
			return std::nullopt;
		}

		return *child;
	}

	std::optional<GmodNode> GmodNode::productSelection() const noexcept
	{
		if ( m_children.size() != 1 )
		{
			return std::nullopt;
		}

		if ( !nfx::string::contains( m_metadata.category(), internal::constants::gmodnode::CATEGORY_FUNCTION ) )
		{
			return std::nullopt;
		}

		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return std::nullopt;
		}

		if ( !nfx::string::contains( child->m_metadata.category(), internal::constants::gmodnode::CATEGORY_PRODUCT ) )
		{
			return std::nullopt;
		}

		if ( child->m_metadata.type() != internal::constants::gmodnode::TYPE_SELECTION )
		{
			return std::nullopt;
		}

		return *child;
	}

	//----------------------------------------------
	// Node type checking methods
	//----------------------------------------------

	bool GmodNode::isIndividualizable( bool isTargetNode, bool isInSet ) const noexcept
	{
		if ( m_metadata.type() == internal::constants::gmodnode::TYPE_GROUP )
		{
			return false;
		}
		if ( m_metadata.type() == internal::constants::gmodnode::TYPE_SELECTION )
		{
			return false;
		}
		if ( isProductType() )
		{
			return false;
		}
		if ( m_metadata.category() == internal::constants::gmodnode::CATEGORY_ASSET && m_metadata.type() == internal::constants::gmodnode::TYPE_TYPE )
		{
			return false;
		}
		if ( isFunctionComposition() )
		{
			if ( m_code.empty() )
			{
				return false;
			}

			return m_code.back() == 'i' || isInSet || isTargetNode;
		}

		return true;
	}

	bool GmodNode::isFunctionComposition() const noexcept
	{
		return (
				   m_metadata.category() == internal::constants::gmodnode::ASSET_FUNCTION ||
				   m_metadata.category() == internal::constants::gmodnode::CATEGORY_PRODUCT_FUNCTION ) &&
			   m_metadata.type() == internal::constants::gmodnode::TYPE_COMPOSITION;
	}

	bool GmodNode::isMappable() const noexcept
	{
		auto productTypeOpt = productType();
		if ( productTypeOpt.has_value() )
		{
			return false;
		}

		auto productSelectionOpt = productSelection();
		if ( productSelectionOpt.has_value() )
		{
			return false;
		}

		if ( m_metadata.category().find( internal::constants::gmodnode::CATEGORY_PRODUCT ) != std::string::npos &&
			 m_metadata.type() == internal::constants::gmodnode::TYPE_SELECTION )
		{
			return false;
		}

		if ( m_metadata.category() == internal::constants::gmodnode::CATEGORY_ASSET )
		{
			return false;
		}

		if ( m_code.empty() )
		{
			return false;
		}

		char lastChar = m_code.back();
		return lastChar != 'a' && lastChar != 's';
	}

	bool GmodNode::isProductSelection() const
	{
		return Gmod::isProductSelection( m_metadata );
	}

	bool GmodNode::isProductType() const
	{
		return Gmod::isProductType( m_metadata );
	}

	bool GmodNode::isAsset() const
	{
		return Gmod::isAsset( m_metadata );
	}

	bool GmodNode::isLeafNode() const
	{
		return Gmod::isLeafNode( m_metadata );
	}

	bool GmodNode::isFunctionNode() const
	{
		return Gmod::isFunctionNode( m_metadata );
	}

	bool GmodNode::isAssetFunctionNode() const
	{
		return Gmod::isAssetFunctionNode( m_metadata );
	}
}
