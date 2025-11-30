/**
 * @file Codebooks.cpp
 * @brief Implementation of Vessel Information Structure (VIS) codebooks
 */

#include "dnv/vista/sdk/Codebooks.h"

#include "internal/core/Codebook.h"
#include "internal/dto/CodebooksDto.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Codebooks class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Codebooks::Codebooks( VisVersion version, const CodebooksDto& dto )
		: m_visVersion{ version },
		  m_codebooks{}
	{
		const auto& items = dto.items;

		for ( auto typeDto : items )
		{
			Codebook codebook( std::move( typeDto ) );
			const auto index = static_cast<size_t>( codebook.name() ) - 1;
			m_codebooks[index] = std::move( codebook );
		}

		CodebookDto detailDto( std::string{ internal::codebook::NAME_DETAIL }, CodebookDto::ValuesMap{} );
		constexpr auto detailIndex = static_cast<size_t>( CodebookName::Detail ) - 1;
		m_codebooks[detailIndex] = Codebook( std::move( detailDto ) );
	}

	Codebooks::Codebooks( const Codebooks& other )
		: m_visVersion{ other.m_visVersion }
	{
		for ( size_t i = 0; i < m_codebooks.size(); ++i )
		{
			m_codebooks[i] = other.m_codebooks[i];
		}
	}
} // namespace dnv::vista::sdk
