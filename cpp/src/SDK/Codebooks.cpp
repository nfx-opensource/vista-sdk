/**
 * @file Codebooks.cpp
 * @brief Implementation of Codebooks class methods
 */

#include "dnv/vista/sdk/Codebooks.h"
#include "dto/CodebooksDto.h"

#include <stdexcept>

namespace dnv::vista::sdk
{
    Codebooks::Codebooks( VisVersion version, const CodebooksDto& dto ) noexcept
        : m_version{ version }
    {
        for( auto& opt : m_codebooks )
        {
            opt.reset();
        }

        for( const auto& item : dto.items )
        {
            Codebook cb{ item };
            auto idx = static_cast<size_t>( cb.name() ) - 1;
            if( idx < m_codebooks.size() )
            {
                m_codebooks[idx].emplace( std::move( cb ) );
            }
        }

        CodebookDto detailDto{ std::string{ "detail" }, {} };
        Codebook detail{ detailDto };
        auto idx = static_cast<size_t>( detail.name() ) - 1;
        if( idx < m_codebooks.size() )
        {
            m_codebooks[idx].emplace( std::move( detail ) );
        }
    }
} // namespace dnv::vista::sdk
