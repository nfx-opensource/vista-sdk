/**
 * @file Codebook.inl
 * @brief Inline implementations for performance-critical codebook operations
 */

#include <nfx/string/Utils.h>

namespace dnv::vista::sdk
{
	//=====================================================================
	// CodebookStandardValues class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline CodebookStandardValues::CodebookStandardValues( CodebookName name, nfx::containers::StringSet&& standardValues ) noexcept
		: m_name{ name },
		  m_standardValues{ std::move( standardValues ) }
	{
	}

	//----------------------------------------------
	// Public methods
	//----------------------------------------------

	inline size_t CodebookStandardValues::count() const noexcept
	{
		return m_standardValues.size();
	}

	inline bool CodebookStandardValues::contains( std::string_view tagValue ) const noexcept
	{
		if ( m_standardValues.contains( tagValue ) )
		{
			return true;
		}

		if ( m_name == CodebookName::Position )
		{
			return nfx::string::isAllDigits( tagValue );
		}

		return false;
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	inline CodebookStandardValues::Iterator CodebookStandardValues::begin() const noexcept
	{
		return m_standardValues.begin();
	}

	inline CodebookStandardValues::Iterator CodebookStandardValues::end() const noexcept
	{
		return m_standardValues.end();
	}

	//=====================================================================
	// CodebookGroups class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline CodebookGroups::CodebookGroups( nfx::containers::StringSet&& groups ) noexcept
		: m_groups{ std::move( groups ) }
	{
	}

	//----------------------------------------------
	// Public methods
	//----------------------------------------------

	inline size_t CodebookGroups::count() const noexcept
	{
		return m_groups.size();
	}

	inline bool CodebookGroups::contains( std::string_view group ) const noexcept
	{
		return m_groups.contains( group );
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	inline CodebookGroups::Iterator CodebookGroups::begin() const noexcept
	{
		return m_groups.begin();
	}

	inline CodebookGroups::Iterator CodebookGroups::end() const noexcept
	{
		return m_groups.end();
	}

	//=====================================================================
	// Codebook class
	//=====================================================================

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline constexpr CodebookName Codebook::name() const noexcept
	{
		return m_name;
	}

	inline const CodebookGroups& Codebook::groups() const noexcept
	{
		return m_groups;
	}

	inline const CodebookStandardValues& Codebook::standardValues() const noexcept
	{
		return m_standardValues;
	}

	inline const nfx::containers::StringMap<std::vector<std::string>>& Codebook::rawData() const noexcept
	{
		return m_rawData;
	}

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	inline bool Codebook::hasGroup( std::string_view group ) const noexcept
	{
		return m_groups.contains( group );
	}

	inline bool Codebook::hasStandardValue( std::string_view value ) const noexcept
	{
		return m_standardValues.contains( value );
	}
}
