/**
 * @file GmodNode.inl
 * @brief Inline implementations for performance-critical GmodNode operations
 */

#include <nfx/string/StringBuilder.h>
#include <nfx/string/Utils.h>

#include "dnv/vista/sdk/config/config.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// GmodNodeMetadata class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodNodeMetadata::GmodNodeMetadata(
		std::string_view category,
		std::string_view type,
		std::string_view name,
		const std::optional<std::string>& commonName,
		const std::optional<std::string>& definition,
		const std::optional<std::string>& commonDefinition,
		const std::optional<bool>& installSubstructure,
		const nfx::containers::FastHashMap<std::string, std::string>& normalAssignmentNames ) noexcept
		: m_category{ category },
		  m_type{ type },
		  m_name{ name },
		  m_commonName{ commonName },
		  m_definition{ definition },
		  m_commonDefinition{ commonDefinition },
		  m_installSubstructure{ installSubstructure },
		  m_normalAssignmentNames{ normalAssignmentNames },
		  m_fullType{ std::string{ category } + " " + std::string{ type } }
	{
	}

	inline GmodNodeMetadata::GmodNodeMetadata( const GmodNodeMetadata& other ) noexcept
		: m_category{ other.m_category },
		  m_type{ other.m_type },
		  m_name{ other.m_name },
		  m_commonName{ other.m_commonName },
		  m_definition{ other.m_definition },
		  m_commonDefinition{ other.m_commonDefinition },
		  m_installSubstructure{ other.m_installSubstructure },
		  m_normalAssignmentNames{ other.m_normalAssignmentNames },
		  m_fullType{ other.m_fullType }
	{
	}

	//----------------------------------------------
	// Assignment operators
	//----------------------------------------------

	inline GmodNodeMetadata& GmodNodeMetadata::operator=( const GmodNodeMetadata& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;
		}

		m_category = other.m_category;
		m_type = other.m_type;
		m_name = other.m_name;
		m_commonName = other.m_commonName;
		m_definition = other.m_definition;
		m_commonDefinition = other.m_commonDefinition;
		m_installSubstructure = other.m_installSubstructure;
		m_normalAssignmentNames = other.m_normalAssignmentNames;
		m_fullType = other.m_fullType;

		return *this;
	}

	//----------------------------------------------
	// Operators
	//----------------------------------------------

	inline bool GmodNodeMetadata::operator==( const GmodNodeMetadata& other ) const
	{
		return nfx::string::equals( m_category, other.m_category ) &&
			   nfx::string::equals( m_type, other.m_type ) &&
			   nfx::string::equals( m_name, other.m_name ) &&
			   m_commonName == other.m_commonName &&
			   m_definition == other.m_definition &&
			   m_commonDefinition == other.m_commonDefinition &&
			   m_installSubstructure == other.m_installSubstructure &&
			   m_normalAssignmentNames == other.m_normalAssignmentNames;
	}

	inline bool GmodNodeMetadata::operator!=( const GmodNodeMetadata& other ) const
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::string_view GmodNodeMetadata::category() const noexcept
	{
		return m_category;
	}

	inline std::string_view GmodNodeMetadata::type() const noexcept
	{
		return m_type;
	}

	inline std::string_view GmodNodeMetadata::fullType() const noexcept
	{
		return m_fullType;
	}

	inline std::string_view GmodNodeMetadata::name() const noexcept
	{
		return m_name;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::commonName() const noexcept
	{
		return m_commonName;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::definition() const noexcept
	{
		return m_definition;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::commonDefinition() const noexcept
	{
		return m_commonDefinition;
	}

	inline const std::optional<bool>& GmodNodeMetadata::installSubstructure() const noexcept
	{
		return m_installSubstructure;
	}

	inline const nfx::containers::FastHashMap<std::string, std::string>& GmodNodeMetadata::normalAssignmentNames() const noexcept
	{
		return m_normalAssignmentNames;
	}

	//=====================================================================
	// GmodNode class
	//=====================================================================

	inline GmodNode::GmodNode( const GmodNode& other ) noexcept
		: m_hashCode{ other.m_hashCode },
		  m_code{ other.m_code },
		  m_location{ other.m_location },
		  m_visVersion{ other.m_visVersion },
		  m_metadata{ other.m_metadata },
		  m_children{ other.m_children },
		  m_parents{ other.m_parents },
		  m_childrenSet{ other.m_childrenSet }
	{
	}

	inline GmodNode::GmodNode( GmodNode&& other ) noexcept
		: m_hashCode{ std::move( other.m_hashCode ) },
		  m_code{ std::move( other.m_code ) },
		  m_location{ std::move( other.m_location ) },
		  m_visVersion{ other.m_visVersion },
		  m_metadata{ std::move( other.m_metadata ) },
		  m_children{ std::move( other.m_children ) },
		  m_parents{ std::move( other.m_parents ) },
		  m_childrenSet{ std::move( other.m_childrenSet ) }
	{
	}

	//----------------------------------------------
	// Assignment operators
	//----------------------------------------------

	inline GmodNode& GmodNode::operator=( const GmodNode& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;
		}

		m_hashCode = other.m_hashCode;
		m_code = other.m_code;
		m_location = other.m_location;
		m_visVersion = other.m_visVersion;
		m_metadata = other.m_metadata;
		m_children = other.m_children;
		m_parents = other.m_parents;
		m_childrenSet = other.m_childrenSet;

		return *this;
	}

	inline GmodNode& GmodNode::operator=( GmodNode&& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;
		}

		m_hashCode = std::move( other.m_hashCode );
		m_code = std::move( other.m_code );
		m_location = std::move( other.m_location );
		m_visVersion = other.m_visVersion;
		m_metadata = std::move( other.m_metadata );
		m_children = std::move( other.m_children );
		m_parents = std::move( other.m_parents );
		m_childrenSet = std::move( other.m_childrenSet );

		return *this;
	}

	//----------------------------------------------
	// Operators
	//----------------------------------------------

	inline bool GmodNode::operator==( const GmodNode& other ) const
	{
		if ( !nfx::string::equals( m_code.code, other.m_code.code ) )
		{
			return false;
		}

		if ( !m_location.has_value() && !other.m_location.has_value() )
		{
			return true;
		}

		return m_location == other.m_location;
	}

	inline bool GmodNode::operator!=( const GmodNode& other ) const
	{
		return !( *this == other );
	}

	inline bool GmodNode::equals( const GmodNode& other ) const
	{
		return ( *this == other );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::string& GmodNode::code() const noexcept
	{
		return m_code.code;
	}

	inline const std::optional<Location>& GmodNode::location() const noexcept
	{
		return m_location;
	}

	inline VisVersion GmodNode::visVersion() const noexcept
	{
		return m_visVersion;
	}

	inline const GmodNodeMetadata& GmodNode::metadata() const noexcept
	{
		return m_metadata;
	}

	//----------------------------------------------
	// Relationship accessors
	//----------------------------------------------

	inline const std::vector<GmodNode*>& GmodNode::children() const noexcept
	{
		return *m_children;
	}

	inline const std::vector<GmodNode*>& GmodNode::parents() const noexcept
	{
		return *m_parents;
	}

	//----------------------------------------------
	// Node type checking methods
	//----------------------------------------------

	inline bool GmodNode::isRoot() const noexcept
	{
		return nfx::string::equals( m_code.code, "VE" );
	}

	//----------------------------------------------
	// Node relationship query methods
	//----------------------------------------------

	inline bool GmodNode::isChild( const GmodNode& node ) const noexcept
	{
		return isChild( node.m_code.code );
	}

	inline bool GmodNode::isChild( std::string_view code ) const noexcept
	{
		return m_childrenSet->contains( code );
	}

	//----------------------------------------------
	// String conversion methods
	//----------------------------------------------

	inline std::string GmodNode::toString() const noexcept
	{
		auto lease = nfx::string::StringBuilderPool::lease();
		lease.create().append( m_code.code );
		if ( m_location.has_value() )
		{
			lease.create().append( '-' );
			lease.create().append( m_location->toString() );
		}

		return lease.toString();
	}

	inline void GmodNode::toString( nfx::string::StringBuilder& builder ) const noexcept
	{
		builder.append( m_code.code );
		if ( m_location.has_value() )
		{
			builder.append( '-' );
			builder.append( m_location->toString() );
		}
	}

	//----------------------------------------------
	// Hashing
	//----------------------------------------------

	inline std::size_t GmodNode::hashCode() const noexcept
	{
		return m_hashCode;
	}

	//----------------------------------------------
	// Relationship management methods
	//----------------------------------------------

	inline void GmodNode::addChild( GmodNode* child ) noexcept
	{
		if ( !child )
		{
			return;
		}

		auto childCode = child->code();
		if ( m_childrenSet->find( childCode ) != nullptr )
		{
			return;
		}

		m_children->push_back( child );
		m_childrenSet->insert( childCode );
	}

	inline void GmodNode::addParent( GmodNode* parent ) noexcept
	{
		if ( !parent )
		{
			return;
		}

		m_parents->push_back( parent );
	}

	inline void GmodNode::trim() noexcept
	{
		m_children->shrink_to_fit();
		m_parents->shrink_to_fit();

		if ( m_childrenSet->size() != m_children->size() )
		{
			m_childrenSet->clear();
			m_childrenSet->reserve( m_children->size() );

			for ( const auto* child : *m_children )
			{
				m_childrenSet->insert( child->code() );
			}
		}
	}
} // namespace dnv::vista::sdk
