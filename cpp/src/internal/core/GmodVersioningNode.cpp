/**
 * @file GmodVersioningNode.cpp
 * @brief Implementation of GMOD versioning node operations
 * @details Contains implementation details and internal conversion logic.
 */

#include "GmodVersioningNode.h"

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// Conversion utilities
	//=====================================================================

	/**
	 * @brief Parse a conversion type from a string
	 */
	static GmodNodeConversion::Type parseConversionType( std::string_view type )
	{
		if ( nfx::string::equals( type, "changeCode" ) )
		{
			return GmodNodeConversion::Type::ChangeCode;
		}
		if ( nfx::string::equals( type, "move" ) )
		{
			return GmodNodeConversion::Type::Move;
		}
		if ( nfx::string::equals( type, "merge" ) )
		{
			return GmodNodeConversion::Type::Merge;
		}
		if ( nfx::string::equals( type, "assignmentChange" ) )
		{
			return GmodNodeConversion::Type::AssignmentChange;
		}
		if ( nfx::string::equals( type, "assignmentDelete" ) )
		{
			return GmodNodeConversion::Type::AssignmentDelete;
		}

		throw std::invalid_argument{ "Invalid conversion type: " + std::string{ type } };
	}

	//=====================================================================
	// GmodVersioningNode class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodVersioningNode::GmodVersioningNode( VisVersion visVersion, const nfx::containers::StringMap<GmodNodeConversionDto>& dto )
		: m_visVersion{ visVersion }
	{
		for ( const auto& [code, dtoNode] : dto )
		{
			GmodNodeConversion conversion;
			conversion.source = dtoNode.source();
			if ( nfx::string::isEmpty( dtoNode.target() ) )
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

	VisVersion GmodVersioningNode::visVersion() const
	{
		return m_visVersion;
	}

	bool GmodVersioningNode::tryGetCodeChanges(
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
