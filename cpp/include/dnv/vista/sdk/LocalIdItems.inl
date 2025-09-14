/**
 * @file LocalIdItems.inl
 * @brief Inline implementations for performance-critical LocalIdItems operations
 */

namespace dnv::vista::sdk
{
	//=====================================================================
	// LocalIdItems class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline LocalIdItems::LocalIdItems( GmodPath&& primaryItem, std::optional<GmodPath>&& secondaryItem )
		: m_primaryItem{ std::move( primaryItem ) },
		  m_secondaryItem{ std::move( secondaryItem ) }
	{
	}

	inline LocalIdItems::LocalIdItems( LocalIdItems&& other, GmodPath&& newPrimaryItem )
		: m_primaryItem{ std::move( newPrimaryItem ) },
		  m_secondaryItem{ std::move( other.m_secondaryItem ) }
	{
	}

	inline LocalIdItems::LocalIdItems( LocalIdItems&& other, std::optional<GmodPath>&& newSecondaryItem )
		: m_primaryItem{ std::move( other.m_primaryItem ) },
		  m_secondaryItem{ std::move( newSecondaryItem ) }
	{
	}

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	inline bool LocalIdItems::operator==( const LocalIdItems& other ) const noexcept
	{
		return m_primaryItem == other.m_primaryItem &&
			   m_secondaryItem == other.m_secondaryItem;
	}

	inline bool LocalIdItems::operator!=( const LocalIdItems& other ) const noexcept
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::optional<GmodPath>& LocalIdItems::primaryItem() const noexcept
	{
		return m_primaryItem;
	}

	inline const std::optional<GmodPath>& LocalIdItems::secondaryItem() const noexcept
	{
		return m_secondaryItem;
	}
}
