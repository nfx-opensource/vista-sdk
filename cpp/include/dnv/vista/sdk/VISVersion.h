#pragma once

#include <cstddef>

namespace dnv::vista::sdk
{
	/**
	 * @brief Enumeration of VIS versions
	 */
	enum class VisVersion : std::uint16_t
	{
		// Todo: Compact std::uint8_t or bitfield encoding
		Unknown = 0,

		v3_4a = 3400,
		v3_5a = 3500,
		v3_6a = 3600,
		v3_7a = 3700,
		v3_8a = 3800,
		v3_9a = 3900,

		Latest = v3_9a
	};

	/**
	 * @brief Pre-increment operator for VisVersion
	 * @param version The version to increment
	 * @return The incremented version
	 */
	inline VisVersion operator++( VisVersion& version );

	/**
	 * @brief Post-increment operator for VisVersion
	 * @param version The version to increment
	 * @return The version before increment
	 */
	inline VisVersion operator++( VisVersion& version, int );

	/**
	 * @brief Addition operator for VisVersion
	 * @param version The base version
	 * @param increment The increment value
	 * @return The version incremented by the specified amount
	 */
	inline VisVersion operator+( VisVersion version, int increment );

	/**
	 * @brief Subtraction operator for VisVersion
	 * @param version The base version
	 * @param decrement The decrement value
	 * @return The version decremented by the specified amount
	 */
	inline VisVersion operator-( VisVersion version, int decrement );

	/**
	 * @brief Less-than-or-equal comparison operator for VisVersion
	 * @param lhs The left-hand side version
	 * @param rhs The right-hand side version
	 * @return true if lhs <= rhs, false otherwise
	 */
	inline bool operator<=( VisVersion lhs, VisVersion rhs );

	/**
	 * @brief Subtraction operator for VisVersion difference
	 * @param lhs The left-hand side version
	 * @param rhs The right-hand side version
	 * @return The numeric difference between versions
	 */
	inline int operator-( VisVersion lhs, VisVersion rhs );

	/**
	 * @brief Static helper class for VisVersion operations
	 */
	class VisVersionExtensions final
	{
	public:
		/**
		 * @brief Check if a VisVersion is valid
		 * @param version The version to check
		 * @return true if valid, false otherwise
		 */
		inline static bool isValid( VisVersion version );

		/**
		 * @brief Get all valid VisVersion values (zero-copy)
		 * @return Const reference to vector containing all valid VisVersion values
		 */
		inline static const std::vector<VisVersion>& allVersions();

		/**
		 * @brief Get the latest available VIS version
		 * @return The latest VIS version
		 */
		inline static VisVersion latestVersion();

		/**
		 * @brief Convert a VisVersion to its string representation
		 * @param version The version to convert
		 * @return String representation of the version
		 */
		inline static std::string toVersionString( VisVersion version );

		/**
		 * @brief Try to parse a string_view to a VisVersion (zero-copy version)
		 * @param versionString The string_view to parse
		 * @param version Output parameter for the parsed version
		 * @return true if parsing succeeded, false otherwise
		 */
		inline static bool tryParse( std::string_view versionString, VisVersion& version );

		/**
		 * @brief Parse a string_view to a VisVersion (zero-copy version)
		 * @param versionString The string_view to parse
		 * @return The parsed VisVersion
		 * @throws std::invalid_argument if parsing fails
		 */
		inline static VisVersion parse( std::string_view versionString );
	};
} // namespace dnv::vista::sdk

#include "detail/VISVersion.inl"
