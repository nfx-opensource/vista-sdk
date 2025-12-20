/**
 * @file GmodNode.inl
 * @brief Inline implementations for GmodNodeMetadata and GmodNode
 */

namespace dnv::vista::sdk
{
	inline bool GmodNodeMetadata::operator==( const GmodNodeMetadata& other ) const noexcept
	{
		return m_category == other.m_category &&
			   m_type == other.m_type &&
			   m_name == other.m_name &&
			   m_commonName == other.m_commonName &&
			   m_definition == other.m_definition &&
			   m_commonDefinition == other.m_commonDefinition &&
			   m_installSubstructure == other.m_installSubstructure &&
			   m_normalAssignmentNames == other.m_normalAssignmentNames;
	}

	inline bool GmodNodeMetadata::operator!=( const GmodNodeMetadata& other ) const noexcept
	{
		return !( *this == other );
	}

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

	inline std::optional<bool> GmodNodeMetadata::installSubstructure() const noexcept
	{
		return m_installSubstructure;
	}

	inline const HashMap<std::string, std::string>& GmodNodeMetadata::normalAssignmentNames() const noexcept
	{
		return m_normalAssignmentNames;
	}

	inline bool GmodNode::operator==( const GmodNode& other ) const noexcept
	{
		if ( m_code != other.m_code )
		{
			return false;
		}

		if ( !m_location.has_value() && !other.m_location.has_value() )
		{
			return true;
		}

		return m_location == other.m_location;
	}

	inline bool GmodNode::operator!=( const GmodNode& other ) const noexcept
	{
		return !( *this == other );
	}

	inline VisVersion GmodNode::version() const noexcept
	{
		return m_visVersion;
	}

	inline std::string_view GmodNode::code() const noexcept
	{
		return m_code;
	}

	inline const std::optional<Location>& GmodNode::location() const noexcept
	{
		return m_location;
	}

	inline const GmodNodeMetadata& GmodNode::metadata() const noexcept
	{
		return *m_metadata;
	}

	inline const std::vector<GmodNode*>& GmodNode::children() const noexcept
	{
		return m_children;
	}

	inline const std::vector<GmodNode*>& GmodNode::parents() const noexcept
	{
		return m_parents;
	}

	inline bool GmodNode::isProductSelection() const noexcept
	{
		return m_metadata->category() == "PRODUCT" && m_metadata->type() == "SELECTION";
	}

	inline bool GmodNode::isProductType() const noexcept
	{
		return m_metadata->category() == "PRODUCT" && m_metadata->type() == "TYPE";
	}

	inline bool GmodNode::isAsset() const noexcept
	{
		return m_metadata->category() == "ASSET";
	}

	inline bool GmodNode::isLeafNode() const noexcept
	{
		return m_metadata->fullType() == "ASSET FUNCTION LEAF" || m_metadata->fullType() == "PRODUCT FUNCTION LEAF";
	}

	inline bool GmodNode::isFunctionNode() const noexcept
	{
		return m_metadata->category() != "PRODUCT" && m_metadata->category() != "ASSET";
	}

	inline bool GmodNode::isAssetFunctionNode() const noexcept
	{
		return m_metadata->category() == "ASSET FUNCTION";
	}

	inline bool GmodNode::isRoot() const noexcept
	{
		return m_code == "VE";
	}

	inline bool GmodNode::isChild( const GmodNode& node ) const noexcept
	{
		return isChild( node.m_code );
	}

	inline bool GmodNode::isChild( std::string_view code ) const noexcept
	{
		if ( !m_childrenSet )
		{
			return false;
		}
		return m_childrenSet->contains( code );
	}

	inline std::string GmodNode::toString() const noexcept
	{
		if ( !m_location.has_value() )
		{
			return m_code;
		}
		return m_code + "-" + m_location->value();
	}

	inline bool GmodNode::isIndividualizable( bool isTargetNode, bool isInSet ) const noexcept
	{
		if ( m_metadata->type() == "GROUP" )
		{
			return false;
		}
		if ( m_metadata->type() == "SELECTION" )
		{
			return false;
		}
		if ( isProductType() )
		{
			return false;
		}
		if ( m_metadata->category() == "ASSET" && m_metadata->type() == "TYPE" )
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

	inline void GmodNode::setLocation( const Location& location ) noexcept
	{
		m_location = location;
	}

	inline void GmodNode::setLocation( Location&& location ) noexcept
	{
		m_location = std::move( location );
	}
} // namespace dnv::vista::sdk
