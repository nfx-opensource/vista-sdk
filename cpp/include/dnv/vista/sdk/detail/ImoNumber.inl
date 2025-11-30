/**
 * @file ImoNumber.inl
 * @brief Inline implementations for performance-critical ImoNumber operations
 */

namespace dnv::vista::sdk
{
	//=====================================================================
	// ImoNumber class
	//=====================================================================

	//----------------------------------------------
	// Operators
	//----------------------------------------------

	inline bool ImoNumber::operator==( const ImoNumber& other ) const noexcept
	{
		return m_value == other.m_value;
	}

	inline bool ImoNumber::operator!=( const ImoNumber& other ) const noexcept
	{
		return !( *this == other );
	}

	inline ImoNumber::operator int() const noexcept
	{
		return m_value;
	}

	//----------------------------------------------
	// Hashing
	//----------------------------------------------

	inline std::size_t ImoNumber::hashCode() const noexcept
	{
		return m_hashCode;
	}
} // namespace dnv::vista::sdk
