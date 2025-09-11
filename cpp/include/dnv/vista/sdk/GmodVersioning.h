/**
 * @file GmodVersioning.h
 * @brief Defines the GmodVersioning class for converting GMOD entities between VIS versions.
 * @details This file contains the declaration of the GmodVersioning class, which is responsible
 *          for applying versioning rules to Generic Product Model (GMOD) nodes, paths, and
 *          LocalIds to transform them from one Vessel Information Structure (VIS) version
 *          to another. It uses GMOD versioning data (typically from JSON files) to perform
 *          these conversions.
 */

#pragma once

#include <nfx/containers/HashMap.h>
#include <nfx/string/Utils.h>

#include "GmodPath.h"
#include "GmodVersioningDto.h"
#include "LocalId.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	enum class VisVersion;

	//=====================================================================
	// GmodVersioning class
	//=====================================================================

	/**
	 * @brief Class responsible for converting GMOD objects to a higher version
	 */
	class GmodVersioning final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor
		 *
		 * @param dto Dictionary of GmodVersioningDto objects indexed by version string with heterogeneous lookup
		 */
		explicit GmodVersioning( const nfx::containers::StringMap<GmodVersioningDto>& dto );

		/** @brief Default constructor. */
		GmodVersioning() = delete;

		/** @brief Copy constructor */
		GmodVersioning( const GmodVersioning& ) = delete;

		/** @brief Move constructor */
		GmodVersioning( GmodVersioning&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~GmodVersioning();

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		GmodVersioning& operator=( const GmodVersioning& ) = delete;

		/** @brief Move assignment operator */
		GmodVersioning& operator=( GmodVersioning&& ) noexcept = delete;

		//----------------------------------------------
		// Conversion
		//----------------------------------------------

		/**
		 * @brief Convert a GmodNode from one version to a higher version
		 * @throws std::invalid_argument If targetVersion is not higher than sourceVersion
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::optional<GmodNode> convertNode( VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const;

		/**
		 * @brief Convert a GmodNode with cached target GMOD
		 * @throws std::invalid_argument If targetVersion is not higher than sourceVersion
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::optional<GmodNode> convertNode(
			VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion, const Gmod& targetGmod ) const;

		/**
		 * @brief Converts a GmodPath from a source VIS version to a target VIS version.
		 * @param sourceVersion The VIS version of the sourcePath.
		 * @param sourcePath The GmodPath to convert (passed by const reference).
		 * @param targetVersion The target VIS version.
		 * @return An optional containing the converted GmodPath if successful, otherwise std::nullopt.
		 * @throws std::invalid_argument if source or target versions are invalid or source >= target.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::optional<GmodPath> convertPath( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const;

		/**
		 * @brief Convert a LocalIdBuilder from one version to a higher version
		 */
		std::optional<LocalIdBuilder> convertLocalId( const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const;

		/**
		 * @brief Convert a LocalId from one version to a higher version
		 */
		std::optional<LocalId> convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion ) const;

	private:
		//----------------------------------------------
		// GmodVersioningMap pimpl
		//----------------------------------------------

		void* m_impl;
	};
}
