/**
 * @file ImoNumber.inl
 * @brief Inline implementations for ImoNumber class
 * @details Provides inline implementations for simple accessors and comparison operators.
 */

namespace dnv::vista::sdk
{
    inline bool ImoNumber::operator==( const ImoNumber& other ) const noexcept
    {
        return m_value == other.m_value;
    }

    inline bool ImoNumber::operator!=( const ImoNumber& other ) const noexcept
    {
        return !( *this == other );
    }

    inline constexpr bool ImoNumber::isValid( int imoNumber ) noexcept
    {
        if( imoNumber < 1000000 || imoNumber > 9999999 )
        {
            return false;
        }

        int digits[7];
        int temp = imoNumber;
        for( int i = 0; i < 7; ++i )
        {
            digits[i] = temp % 10;
            temp /= 10;
        }

        int checkSum = 0;
        for( int i = 1; i < 7; ++i )
        {
            checkSum += digits[i] * ( i + 1 );
        }

        int calculatedCheckDigit = checkSum % 10;
        int providedCheckDigit = digits[0];

        return providedCheckDigit == calculatedCheckDigit;
    }
} // namespace dnv::vista::sdk
