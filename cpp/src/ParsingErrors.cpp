/**
 * @file ParsingErrors.cpp
 * @brief Implementation of ParsingErrors class
 */

#include <stdexcept>

#include <nfx/Hashing.h>

#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/config/config.h"

namespace dnv::vista::sdk
{
	namespace internal
	{
		//----------------------------------------------
		// Hashing
		//----------------------------------------------

		/**
		 * @brief Computes hash value for ParsingErrors by combining all error entries
		 * @details Combines hash codes of all ErrorEntry type and message strings using
		 *          NFX optimized hash combination for consistent results across
		 *          different error collections and diagnostic scenarios.
		 */
		static size_t computeHash( const std::vector<ParsingErrors::ErrorEntry>& errors ) noexcept
		{
			uint32_t hashCode = VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS;

			// Hash each error entry (type + message)
			for ( const auto& error : errors )
			{
				// Hash the error type
				uint32_t typeHash = nfx::hashing::hash<std::string_view, uint32_t, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS>( error.type );
				hashCode = nfx::hashing::combine<uint32_t>( hashCode, typeHash );

				// Hash the error message
				uint32_t messageHash = nfx::hashing::hash<std::string_view, uint32_t, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS>( error.message );
				hashCode = nfx::hashing::combine<uint32_t>( hashCode, messageHash );
			}

			return static_cast<size_t>( hashCode );
		}
	} // namespace internal

	//=====================================================================
	// ParsingErrors class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	ParsingErrors::ParsingErrors( std::vector<ErrorEntry>&& errors ) noexcept
		: m_hashCode{ internal::computeHash( errors ) },
		  m_errors{ std::move( errors ) }
	{
	}

	ParsingErrors::ParsingErrors()
		: m_hashCode{ VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS },
		  m_errors{}
	{
	}

	//----------------------------------------------
	// String conversion methods
	//----------------------------------------------

	std::string ParsingErrors::toString() const
	{
		if ( m_errors.empty() )
		{
			return "Success";
		}

		constexpr std::string_view header = "Parsing errors:\n";

		// Pre-calculate exact capacity
		size_t capacity = header.size();
		for ( const auto& error : m_errors )
		{
			capacity += 1 + error.type.size() + 3 + error.message.size() + 1;
			//          ↑                       ↑                          ↑
			//        '\t'                    " - "                      '\n'
		}

		std::string result;
		result.reserve( capacity );
		result = header;

		for ( const auto& error : m_errors )
		{
			result += '\t';
			result += error.type;
			result += " - ";
			result += error.message;
			result += '\n';
		}

		return result;
	}

	//----------------------------------------------
	// Enumeration
	//----------------------------------------------

	ParsingErrors::Enumerator ParsingErrors::enumerator() const
	{
		return Enumerator( &m_errors );
	}

	//----------------------------------------------
	// ParsingErrors::Enumerator class
	//----------------------------------------------

	//----------------------------
	// Construction
	//----------------------------

	ParsingErrors::Enumerator::Enumerator( const std::vector<ErrorEntry>* data )
		: m_data{ data },
		  m_index{ 0 }
	{
	}

	//----------------------------
	// Enumeration methods
	//----------------------------

	const ParsingErrors::ErrorEntry& ParsingErrors::Enumerator::current() const
	{
		if ( m_index == 0 || m_index > m_data->size() )
		{
			throw std::out_of_range{ "Enumerator not positioned on valid element" };
		}

		return ( *m_data )[m_index - 1];
	}

	//----------------------------------------------
	// ParsingErrors::ErrorEntry struct
	//----------------------------------------------

	ParsingErrors::ErrorEntry::ErrorEntry( std::string_view errorType, std::string_view errorMessage )
		: type{ errorType },
		  message{ errorMessage }
	{
	}

	ParsingErrors::ErrorEntry::ErrorEntry( std::string&& errorType, std::string&& errorMessage )
		: type{ std::move( errorType ) },
		  message{ std::move( errorMessage ) }
	{
	}
} // namespace dnv::vista::sdk
