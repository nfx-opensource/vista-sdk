/**
 * @file LocalIdItems.cpp
 * @brief Implementation of the LocalIdItems class
 */

#include <cctype>
#include <optional>
#include <string>
#include <string_view>

#include <nfx/string/StringBuilderPool.h>
#include <nfx/string/Utils.h>

#include "dnv/vista/sdk/LocalIdItems.h"

#include "internal/core/LocalId.h"

namespace dnv::vista::sdk
{
	namespace internal
	{
		//=====================================================================
		// Internal helper functions
		//=====================================================================

		static void appendCommonName(
			nfx::string::StringBuilder& builder,
			std::string_view commonName,
			const std::optional<std::string>& location )
		{
			char prev = '\0';
			for ( const char ch : commonName )
			{
				if ( ch == '/' || ( prev == ' ' && ch == ' ' ) )
				{
					continue;
				}

				char current;
				if ( ch == ' ' )
				{
					current = '.';
				}
				else if ( !nfx::string::isURIUnreserved( ch ) )
				{
					current = '.';
				}
				else
				{
					current = nfx::string::toLower( ch );
				}

				if ( current == '.' && prev == '.' )
				{
					continue;
				}

				builder.push_back( current );
				prev = current;
			}

			if ( location.has_value() && !nfx::string::isEmpty( *location ) )
			{
				builder.push_back( '.' );
				builder.append( *location );
			}
		}
	}

	//=====================================================================
	// LocalIdItems class
	//=====================================================================

	//----------------------------------------------
	// Public methods
	//----------------------------------------------

	void LocalIdItems::append(
		nfx::string::StringBuilder& builder,
		bool verboseMode ) const
	{
		if ( !m_primaryItem && !m_secondaryItem )
		{
			return;
		}

		if ( m_primaryItem )
		{
			m_primaryItem->toString( builder );
			builder.push_back( '/' );
		}

		if ( m_secondaryItem )
		{
			builder.append( internal::localId::PREFIX_SEC );
			builder.push_back( '/' );
			m_secondaryItem->toString( builder );
			builder.push_back( '/' );
		}

		if ( verboseMode )
		{
			if ( m_primaryItem )
			{
				for ( const auto& [depth, name] : m_primaryItem->commonNames() )
				{
					builder.push_back( '~' );
					const GmodNode& nodeRef = ( *m_primaryItem )[depth];
					if ( nodeRef.location().has_value() )
					{
						const auto locationStr = nodeRef.location()->toString();
						internal::appendCommonName( builder, name, std::optional<std::string>( locationStr ) );
					}
					else
					{
						internal::appendCommonName( builder, name, std::nullopt );
					}
					builder.push_back( '/' );
				}
			}

			if ( m_secondaryItem )
			{
				bool isFirstSecondary = true;
				for ( const auto& [depth, name] : m_secondaryItem->commonNames() )
				{
					if ( isFirstSecondary )
					{
						builder.append( internal::localId::PREFIX_FOR );
						isFirstSecondary = false;
					}
					else
					{
						builder.append( internal::localId::PREFIX_TILDE );
					}

					const GmodNode& nodeRef = ( *m_secondaryItem )[depth];
					if ( nodeRef.location().has_value() )
					{
						const auto locationStr = nodeRef.location()->toString();
						internal::appendCommonName( builder, name, std::optional<std::string>( locationStr ) );
					}
					else
					{
						internal::appendCommonName( builder, name, std::nullopt );
					}
					builder.push_back( '/' );
				}
			}
		}
	}
}
