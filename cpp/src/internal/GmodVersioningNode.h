/**
 * @file GmodVersioningNode.h
 * @brief Internal implementation of GMOD versioning node for converting between VIS versions
 * @details Contains private types and implementation details for GmodVersioning functionality.
 */

#pragma once

#include <optional>
#include <stdexcept>
#include <tuple>
#include <string_view>

#include <nfx/containers/StringMap.h>

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/Locations.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	struct GmodNodeConversion;

	//=====================================================================
	// GmodVersioningNode class
	//=====================================================================

	/**
	 * @brief Class representing versioning information for a specific VIS version
	 */
	class GmodVersioningNode final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor for GmodVersioningNode
		 */
		inline explicit GmodVersioningNode( VisVersion visVersion, const nfx::containers::StringMap<GmodNodeConversionDto>& dto );

		/** @brief Default constructor. */
		GmodVersioningNode() = default;

		/** @brief Copy constructor */
		GmodVersioningNode( const GmodVersioningNode& ) = default;

		/** @brief Move constructor */
		GmodVersioningNode( GmodVersioningNode&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~GmodVersioningNode() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		GmodVersioningNode& operator=( const GmodVersioningNode& ) = default;

		/** @brief Move assignment operator */
		GmodVersioningNode& operator=( GmodVersioningNode&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the VIS version of this versioning node
		 */
		inline VisVersion visVersion() const;

		/**
		 * @brief Try to get code changes for a specific node code
		 */
		inline bool tryGetCodeChanges( std::string_view code, const GmodNodeConversion*& nodeChanges ) const;

	private:
		//----------------------------
		// Private member variables
		//----------------------------

		VisVersion m_visVersion;
		nfx::containers::StringMap<GmodNodeConversion> m_versioningNodeChanges;
	};
}

#include "GmodVersioningNode.inl"
