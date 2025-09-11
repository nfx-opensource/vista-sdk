/**
 * @file GmodVersioningNode.inl
 * @brief Implementation details for GMOD versioning node operations
 * @details Contains inline implementations and internal conversion logic.
 */

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// Conversion operation types, structures and utilities
	//=====================================================================

	/**
	 * @brief Enumeration of conversion operation types
	 */
	enum class ConversionType
	{
		ChangeCode = 0,
		Merge,
		Move,
		AssignmentChange = 20,
		AssignmentDelete = 21
	};

	/**
	 * @brief Structure holding information about a node conversion
	 */
	struct GmodNodeConversion final
	{
		std::unordered_set<ConversionType> operations;
		std::string source;
		std::optional<std::string> target;
		std::optional<std::string> oldAssignment;
		std::optional<std::string> newAssignment;
		std::optional<bool> deleteAssignment;
	};

	/**
	 * @brief Parse a conversion type from a string
	 */
	static ConversionType parseConversionType( std::string_view type )
	{
		if ( type == "changeCode" )
		{
			return ConversionType::ChangeCode;
		}
		if ( type == "move" )
		{
			return ConversionType::Move;
		}
		if ( type == "merge" )
		{
			return ConversionType::Merge;
		}
		if ( type == "assignmentChange" )
		{
			return ConversionType::AssignmentChange;
		}
		if ( type == "assignmentDelete" )
		{
			return ConversionType::AssignmentDelete;
		}

		throw std::invalid_argument{ "Invalid conversion type: " + std::string{ type } };
	}

	//=====================================================================
	// GmodVersioningNode class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodVersioningNode::GmodVersioningNode( VisVersion visVersion, const nfx::containers::StringMap<GmodNodeConversionDto>& dto )
		: m_visVersion{ visVersion }
	{
		for ( const auto& [code, dtoNode] : dto )
		{
			GmodNodeConversion conversion;
			conversion.source = dtoNode.source();
			if ( dtoNode.target().empty() )
			{
				conversion.target = std::nullopt;
			}
			else
			{
				conversion.target = dtoNode.target();
			}
			conversion.oldAssignment = dtoNode.oldAssignment();
			conversion.newAssignment = dtoNode.newAssignment();
			conversion.deleteAssignment = dtoNode.deleteAssignment();

			if ( !dtoNode.operations().empty() )
			{
				for ( const auto& type : dtoNode.operations() )
				{
					conversion.operations.insert( parseConversionType( type ) );
				}
			}

			m_versioningNodeChanges.emplace( code, conversion );
		}
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline VisVersion GmodVersioningNode::visVersion() const
	{
		return m_visVersion;
	}

	inline bool GmodVersioningNode::tryGetCodeChanges(
		std::string_view code,
		const GmodNodeConversion*& nodeChanges ) const
	{
		auto it = m_versioningNodeChanges.find( code );
		if ( it != m_versioningNodeChanges.end() )
		{
			nodeChanges = &it->second;

			return true;
		}

		return false;
	}
}
