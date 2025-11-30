/**
 * @file GmodNode.cpp
 * @brief Implementation of the GmodNode and GmodNodeMetadata classes
 */

#include <nfx/Hashing.h>
#include <nfx/string/Utils.h>

#include "dnv/vista/sdk/GmodNode.h"

#include "internal/core/GmodNode.h"
#include "internal/dto/GmodDto.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	namespace internal
	{
		//=====================================================================
		// Hash computation
		//=====================================================================

		/**
		 * @brief Updates the cached hash code based on current code and location values.
		 */
		static std::size_t updateHashCode( std::size_t codeHash, const std::optional<Location>& location ) noexcept
		{
			if ( !location.has_value() )
			{
				return codeHash;
			}

			auto locationHash = location.value().hashCode();

			return nfx::hashing::combine( codeHash, locationHash );
		}
	} // namespace internal

	//=====================================================================
	// GmodNode class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodNode::GmodNode( VisVersion version, const GmodNodeDto& dto ) noexcept
		: m_code{ std::string{ dto.code },
			  static_cast<std::size_t>( nfx::hashing::hash<std::string_view, uint32_t>( dto.code ) ) },
		  m_location{ std::nullopt },
		  m_visVersion{ version },
		  m_metadata{
			  dto.category,
			  dto.type,
			  dto.name,
			  dto.commonName,
			  dto.definition,
			  dto.commonDefinition,
			  dto.installSubstructure,
			  dto.normalAssignmentNames.has_value() ? *dto.normalAssignmentNames
													: nfx::containers::FastHashMap<std::string, std::string>() },
		  m_children{ std::make_shared<std::vector<GmodNode*>>() },
		  m_parents{ std::make_shared<std::vector<GmodNode*>>() },
		  m_childrenSet{ std::make_shared<nfx::containers::FastHashSet<std::string>>() }
	{
		m_hashCode = internal::updateHashCode( m_code.hash, m_location );
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

		result.m_hashCode = internal::updateHashCode( m_code.hash, result.m_location );

		return result;
	}

	GmodNode GmodNode::withLocation( std::string_view locationStr ) const
	{
		Locations locations = VIS::instance().locations( m_visVersion );
		Location location = locations.parse( locationStr );

		return withLocation( location );
	}

	GmodNode GmodNode::withLocation( const Location& location ) const
	{
		GmodNode result = *this;
		result.m_location = location;

		result.m_hashCode = internal::updateHashCode( m_code.hash, result.m_location );

		return result;
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
		if ( m_children->size() != 1 )
		{
			return std::nullopt;
		}

		if ( !nfx::string::contains( m_metadata.category(), internal::gmodnode::CATEGORY_FUNCTION ) )
		{
			return std::nullopt;
		}

		const GmodNode* child = ( *m_children )[0];
		if ( !child )
		{
			return std::nullopt;
		}

		if ( !nfx::string::equals( child->m_metadata.category(), internal::gmodnode::CATEGORY_PRODUCT ) )
		{
			return std::nullopt;
		}

		if ( !nfx::string::equals( child->m_metadata.type(), internal::gmodnode::TYPE_TYPE ) )
		{
			return std::nullopt;
		}

		return *child;
	}

	std::optional<GmodNode> GmodNode::productSelection() const noexcept
	{
		if ( m_children->size() != 1 )
		{
			return std::nullopt;
		}

		if ( !nfx::string::contains( m_metadata.category(), internal::gmodnode::CATEGORY_FUNCTION ) )
		{
			return std::nullopt;
		}

		const GmodNode* child = ( *m_children )[0];
		if ( !child )
		{
			return std::nullopt;
		}

		if ( !nfx::string::contains( child->m_metadata.category(), internal::gmodnode::CATEGORY_PRODUCT ) )
		{
			return std::nullopt;
		}

		if ( !nfx::string::equals( child->m_metadata.type(), internal::gmodnode::TYPE_SELECTION ) )
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
		if ( nfx::string::equals( m_metadata.type(), internal::gmodnode::TYPE_GROUP ) )
		{
			return false;
		}
		if ( nfx::string::equals( m_metadata.type(), internal::gmodnode::TYPE_SELECTION ) )
		{
			return false;
		}
		if ( isProductType() )
		{
			return false;
		}
		if ( nfx::string::equals( m_metadata.category(), internal::gmodnode::CATEGORY_ASSET ) && nfx::string::equals( m_metadata.type(), internal::gmodnode::TYPE_TYPE ) )
		{
			return false;
		}
		if ( isFunctionComposition() )
		{
			if ( nfx::string::isEmpty( m_code.code ) )
			{
				return false;
			}

			return m_code.code.back() == 'i' || isInSet || isTargetNode;
		}

		return true;
	}

	bool GmodNode::isFunctionComposition() const noexcept
	{
		return (
				   nfx::string::equals( m_metadata.category(), internal::gmodnode::ASSET_FUNCTION ) ||
				   nfx::string::equals( m_metadata.category(), internal::gmodnode::CATEGORY_PRODUCT_FUNCTION ) ) &&
			   nfx::string::equals( m_metadata.type(), internal::gmodnode::TYPE_COMPOSITION );
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

		if ( nfx::string::contains( m_metadata.category(), internal::gmodnode::CATEGORY_PRODUCT ) &&
			 nfx::string::equals( m_metadata.type(), internal::gmodnode::TYPE_SELECTION ) )
		{
			return false;
		}

		if ( nfx::string::equals( m_metadata.category(), internal::gmodnode::CATEGORY_ASSET ) )
		{
			return false;
		}

		if ( nfx::string::isEmpty( m_code.code ) )
		{
			return false;
		}

		char lastChar = m_code.code.back();
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
} // namespace dnv::vista::sdk
