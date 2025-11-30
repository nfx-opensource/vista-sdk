/**
 * @file ImoNumber.cpp
 * @brief Implementation of the ImoNumber class
 */

#include <stdexcept>
#include <string>

#include <nfx/Hashing.h>
#include <nfx/StringUtils.h>

#include "dnv/vista/sdk/ImoNumber.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// ImoNumber class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	ImoNumber::ImoNumber()
		: m_hashCode{ 0 },
		  m_value{ 0 }
	{
	}

	ImoNumber::ImoNumber( int value )
	{
		if ( !isValid( value ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::to_string( value ) };
		}

		m_value = value;

		m_hashCode = nfx::hashing::hash<uint32_t, uint32_t, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS>( static_cast<uint32_t>( value ) );
	}

	ImoNumber::ImoNumber( std::string_view value )
	{
		if ( nfx::string::isEmpty( value ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
		}

		if ( nfx::string::isNullOrWhiteSpace( value ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
		}

		std::string_view sv = value;
		if ( sv.length() >= 3 && nfx::string::iequals( sv.substr( 0, 3 ), "IMO" ) )
		{
			sv = sv.substr( 3 );
		}

		int num = 0;
		if ( !nfx::string::fromString<int>( sv, num ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
		}
		if ( num == 0 || !isValid( num ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
		}

		m_value = num;
		m_hashCode = nfx::hashing::hash<uint32_t, uint32_t, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS>( static_cast<uint32_t>( num ) );
	}

	//----------------------------------------------

	// String conversion
	//----------------------------------------------

	std::string ImoNumber::toString() const
	{
		return "IMO" + std::to_string( m_value );
	}

	//----------------------------------------------
	// State inspection
	//----------------------------------------------

	/*
			IMO number validation according to the standard:
			https://en.wikipedia.org/wiki/IMO_number
			An IMO number is made of the three letters "IMO" followed by a seven-digit number.
			This consists of a six-digit sequential unique number followed by a check digit.
			The integrity of an IMO number can be verified using its check digit.
			This is done by multiplying each of the first six digits by a factor
			of 2 to 7 corresponding to their position from right to left.
			The rightmost digit of this sum is the check digit.
			For example, for IMO 9074729: (9×7) + (0×6) + (7×5) + (4×4) + (7×3) + (2×2) = 139
			The rightmost digit (9) must equal checksum mod 10 (139 % 10 = 9)
		*/
	bool ImoNumber::isValid( int imoNumber ) noexcept
	{
		if ( imoNumber < 1000000 || imoNumber > 9999999 )
		{
			return false;
		}

		int digits[7];
		int temp = imoNumber;
		for ( int i = 0; i < 7; ++i )
		{
			digits[i] = temp % 10;
			temp /= 10;
		}

		int checkSum = 0;
		for ( int i = 1; i < 7; ++i )
		{
			checkSum += digits[i] * ( i + 1 );
		}

		int calculatedCheckDigit = checkSum % 10;
		int providedCheckDigit = digits[0];

		return providedCheckDigit == calculatedCheckDigit;
	}

	//----------------------------------------------
	// Parsing
	//----------------------------------------------

	ImoNumber ImoNumber::parse( std::string_view value )
	{
		ImoNumber imo;
		if ( !tryParse( value, imo ) )
		{
			throw std::invalid_argument( "Failed to parse ImoNumber: " + std::string{ value } );
		}
		return imo;
	}

	bool ImoNumber::tryParse( std::string_view value, ImoNumber& imoNumber )
	{
		if ( nfx::string::isEmpty( value ) )
		{
			return false;
		}

		if ( nfx::string::isNullOrWhiteSpace( value ) )
		{
			return false;
		}

		std::string_view sv = value;
		if ( sv.length() >= 3 && nfx::string::iequals( sv.substr( 0, 3 ), "IMO" ) )
		{
			sv = sv.substr( 3 );
		}

		int num = 0;
		if ( !nfx::string::fromString<int>( sv, num ) )
		{
			return false;
		}

		if ( num == 0 || !isValid( num ) )
		{
			return false;
		}

		imoNumber = ImoNumber{ num };

		return true;
	}
} // namespace dnv::vista::sdk
