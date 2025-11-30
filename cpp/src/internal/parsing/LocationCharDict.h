/**
 * @file LocationCharDict.h
 * @brief Dictionary-like structure for managing location characters within parsing logic
 * @details Provides efficient lookup and storage of location characters mapped to LocationGroup keys,
 *          used internally for parsing and validation of location identifiers.
 */

#pragma once

#include <array>
#include <stdexcept>
#include <optional>

#include <nfx/string/StringBuilder.h>

#include "dnv/vista/sdk/Locations.h"

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// LocationCharDict class
	//=====================================================================

	/**
	 * @brief A dictionary-like structure for managing location characters within parsing logic.
	 */
	class LocationCharDict final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor. */
		LocationCharDict() = default;

		/** @brief Copy constructor */
		LocationCharDict( const LocationCharDict& ) = delete;

		/** @brief Move constructor */
		LocationCharDict( LocationCharDict&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~LocationCharDict() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		LocationCharDict& operator=( const LocationCharDict& ) = delete;

		/** @brief Move assignment operator */
		LocationCharDict& operator=( LocationCharDict&& other ) noexcept = default;

		//----------------------------------------------
		// Lookup operators
		//----------------------------------------------

		/**
		 * @brief Access a location character by key
		 * @param key The location group key
		 * @return A reference to the optional character
		 */
		inline std::optional<char>& operator[]( LocationGroup key );

		//----------------------------------------------
		// Public methods
		//----------------------------------------------

		/**
		 * @brief Try to add a value to the dictionary
		 * @param key The location group key
		 * @param value The character value to add
		 * @param existingValue Output parameter for the existing value, if any
		 * @return True if the value was added, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool tryAdd( LocationGroup key, char value, std::optional<char>& existingValue );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The internal table storing optional characters for each relevant `LocationGroup`.
		 * The array size is 4, corresponding to Side, Vertical, Transverse, and Longitudinal groups.
		 * `LocationGroup::Number` is handled separately in parsing logic.
		 */
		std::array<std::optional<char>, 4> m_table;
	};

	//=====================================================================
	// LocationCharDict inline implementations
	//=====================================================================

	//----------------------------------------------
	// Lookup operators
	//----------------------------------------------

	inline std::optional<char>& LocationCharDict::operator[]( LocationGroup key )
	{
		if ( static_cast<int>( key ) <= 0 )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.create();
			builder.append( "Unsupported code: " );
			builder.append( std::to_string( static_cast<int>( key ) ) );

			throw std::runtime_error{ lease.toString() };
		}

		auto index{ static_cast<size_t>( key ) - 1 };
		if ( index >= m_table.size() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.create();
			builder.append( "Unsupported code: " );
			builder.append( std::to_string( static_cast<int>( key ) ) );

			throw std::runtime_error{ lease.toString() };
		}

		return m_table[index];
	}

	//----------------------------------------------
	// Public methods
	//----------------------------------------------

	inline bool LocationCharDict::tryAdd( LocationGroup key, char value, std::optional<char>& existingValue )
	{
		auto& v = ( *this )[key];
		if ( v.has_value() )
		{
			existingValue = v;

			return false;
		}

		existingValue = std::nullopt;
		v = value;

		return true;
	}
} // namespace dnv::vista::sdk::internal
