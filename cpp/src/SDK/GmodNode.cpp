/**
 * @file GmodNode.cpp
 * @brief Implementation of GmodNodeMetadata and GmodNode
 */

#include "dnv/vista/sdk/GmodNode.h"

#include "dto/GmodDto.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	GmodNodeMetadata::GmodNodeMetadata(
		std::string_view category,
		std::string_view type,
		std::string_view name,
		std::optional<std::string> commonName,
		std::optional<std::string> definition,
		std::optional<std::string> commonDefinition,
		std::optional<bool> installSubstructure,
		HashMap<std::string, std::string> normalAssignmentNames ) noexcept
		: m_category{ category },
		  m_type{ type },
		  m_name{ name },
		  m_commonName{ commonName },
		  m_definition{ definition },
		  m_commonDefinition{ commonDefinition },
		  m_installSubstructure{ installSubstructure },
		  m_normalAssignmentNames{ normalAssignmentNames },
		  m_fullType{ std::string{ category } + ' ' + std::string{ type } }
	{
	}

	GmodNode::GmodNode( VisVersion version, const GmodNodeDto& dto ) noexcept
		: m_visVersion{ version },
		  m_code{ dto.code },
		  m_location{ std::nullopt },
		  m_metadata{ new GmodNodeMetadata(
			  dto.category,
			  dto.type,
			  dto.name.value_or( "" ),
			  dto.commonName,
			  dto.definition,
			  dto.commonDefinition,
			  dto.installSubstructure,
			  dto.normalAssignmentNames ? HashMap<std::string, std::string>{
											  dto.normalAssignmentNames->begin(),
											  dto.normalAssignmentNames->end() }
										: HashMap<std::string, std::string>{} ) },
		  m_children{}, m_parents{}, m_childrenSet{ nullptr }
	{
	}

	std::optional<const GmodNode*> GmodNode::productType() const noexcept
	{
		if ( m_children.size() != 1 )
		{
			return std::nullopt;
		}
		if ( m_metadata->category().find( "FUNCTION" ) == std::string_view::npos )
		{
			return std::nullopt;
		}
		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return std::nullopt;
		}
		if ( child->m_metadata->category() != "PRODUCT" )
		{
			return std::nullopt;
		}
		if ( child->m_metadata->type() != "TYPE" )
		{
			return std::nullopt;
		}

		return child;
	}

	std::optional<const GmodNode*> GmodNode::productSelection() const noexcept
	{
		if ( m_children.size() != 1 )
		{
			return std::nullopt;
		}
		if ( m_metadata->category().find( "FUNCTION" ) == std::string_view::npos )
		{
			return std::nullopt;
		}
		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return std::nullopt;
		}
		if ( !nfx::string::contains( child->m_metadata->category(), "PRODUCT" ) )
		{
			return std::nullopt;
		}
		if ( child->m_metadata->type() != "SELECTION" )
		{
			return std::nullopt;
		}

		return child;
	}

	bool GmodNode::isFunctionComposition() const noexcept
	{
		return ( m_metadata->category() == "ASSET FUNCTION" || m_metadata->category() == "PRODUCT FUNCTION" ) &&
			   m_metadata->type() == "COMPOSITION";
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
		if ( nfx::string::contains( m_metadata->category(), "PRODUCT" ) &&
			 m_metadata->type() == "SELECTION" )
		{
			return false;
		}
		if ( m_metadata->category() == "ASSET" )
		{
			return false;
		}
		if ( m_code.empty() )
		{
			return false;
		}

		char lastChar = m_code.back();
		if ( lastChar == 'a' || lastChar == 's' )
		{
			return false;
		}

		return true;
	}

	void GmodNode::toString( StringBuilder& builder ) const noexcept
	{
		builder << m_code;

		if ( m_location.has_value() )
		{
			builder << '-' << m_location->value();
		}
	}

	GmodNode GmodNode::withLocation( const Location& location ) const noexcept
	{
		GmodNode result = *this;
		result.m_location = location;

		return result;
	}

	std::optional<GmodNode> GmodNode::withLocation( std::string_view locationStr ) const
	{
		const Locations& locations = VIS::instance().locations( m_visVersion );
		auto locationOpt = locations.fromString( locationStr );

		if ( !locationOpt.has_value() )
		{
			return std::nullopt;
		}

		return withLocation( locationOpt.value() );
	}

	std::optional<GmodNode> GmodNode::withLocation( std::string_view locationStr, ParsingErrors& errors ) const
	{
		const Locations& locations = VIS::instance().locations( m_visVersion );

		auto locationOpt = locations.fromString( locationStr, errors );
		if ( !locationOpt.has_value() )
		{
			return std::nullopt;
		}

		return withLocation( locationOpt.value() );
	}

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

	void GmodNode::addChild( GmodNode* child ) noexcept
	{
		if ( !child )
		{
			return;
		}

		m_children.push_back( child );
		if ( m_childrenSet )
		{
			m_childrenSet->emplace( child->code() );
		}
	}

	void GmodNode::addParent( GmodNode* parent ) noexcept
	{
		if ( !parent )
		{
			return;
		}

		m_parents.push_back( parent );
	}

	void GmodNode::trim() noexcept
	{
		m_children.shrink_to_fit();
		m_parents.shrink_to_fit();

		if ( !m_childrenSet )
		{
			m_childrenSet = std::make_shared<HashSet<std::string>>();
			m_childrenSet->clear();
			m_childrenSet->reserve( m_children.size() );

			for ( const auto* child : m_children )
			{
				if ( child )
				{
					m_childrenSet->emplace( child->code() );
				}
			}
		}
	}
} // namespace dnv::vista::sdk
