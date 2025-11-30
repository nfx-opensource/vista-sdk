/**
 * @file VIS.inl
 * @brief Inline implementations for performance-critical VIS operations
 */

namespace dnv::vista::sdk
{
	//=====================================================================
	// VIS singleton
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline VIS::VIS()
	{
	}

	//----------------------------------------------
	// Singleton access
	//----------------------------------------------

	inline const VIS& VIS::instance()
	{
		static VIS instance;

		return instance;
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	//-----------------------------
	// VisVersion
	//-----------------------------

	inline VisVersion VIS::latestVisVersion() const noexcept
	{
		return VisVersion::Latest;
	}

	inline const std::vector<VisVersion>& VIS::visVersions() const noexcept
	{
		return VisVersionExtensions::allVersions();
	}
} // namespace dnv::vista::sdk
