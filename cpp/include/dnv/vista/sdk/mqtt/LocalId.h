/**
 * @file LocalId.h
 * @brief MQTT-compatible LocalId implementation with inheritance from main LocalId
 */

#pragma once

#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"

namespace dnv::vista::sdk::mqtt
{
	//=====================================================================
	// LocalId class
	//=====================================================================

	/**
	 * @brief MQTT-compatible LocalId implementation inheriting from main LocalId.
	 * @details This class extends the base LocalId with MQTT-specific formatting capabilities:
	 *          - Underscores instead of slashes in GMOD paths for topic compatibility
	 *          - No leading slash to match MQTT topic conventions
	 *          - No "meta/" prefix section for cleaner IoT topics
	 *          - Placeholder handling for missing components
	 */
	class LocalId final : public sdk::LocalId
	{
		friend class sdk::LocalIdBuilder;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs MQTT LocalId from validated LocalIdBuilder.
		 * @param[in] builder Valid LocalIdBuilder instance.
		 * @throws std::invalid_argument If builder is invalid or empty.
		 */
		explicit LocalId( sdk::LocalIdBuilder builder );

	public:
		/** @brief Default constructor - creates an empty/invalid LocalId - Todo: check this */
		inline LocalId();
		/**
		 * @brief Copy constructor
		 * @param other The MQTT LocalId to copy from
		 */
		LocalId( const LocalId& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The MQTT LocalId to move from
		 */
		LocalId( LocalId&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		virtual ~LocalId() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 */
		LocalId& operator=( const LocalId& ) = delete;

		/**
		 * @brief Move assignment operator
		 * @param other The MQTT LocalId to move from
		 * @return Reference to this LocalId
		 */
		LocalId& operator=( LocalId&& other ) noexcept = default;

		//----------------------------------------------
		// MQTT-specific string conversion
		//----------------------------------------------

		/**
		 * @brief Converts LocalId to MQTT-compatible topic string.
		 * @details Provides MQTT-specific formatting (not override since base isn't virtual):
		 *          - No leading slash
		 *          - Underscores instead of slashes in paths
		 *          - No "meta/" section
		 *          - Placeholders for missing components
		 * @return MQTT-compatible Local ID topic string.
		 */
		[[nodiscard]] std::string toString() const;
	};
} // namespace dnv::vista::sdk::mqtt
