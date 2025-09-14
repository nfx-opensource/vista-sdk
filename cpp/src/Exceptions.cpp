/**
 * @file Exceptions.cpp
 * @brief Implementation of Vista SDK exception classes
 */

#include <vector>

#include <nfx/string/StringBuilderPool.h>

#include "dnv/vista/sdk/Exceptions.h"

namespace dnv::vista::sdk
{
	namespace internal
	{
		//=====================================================================
		// Helper functions
		//=====================================================================

		static std::string joinErrors( const std::vector<std::string>& errors )
		{
			if ( errors.empty() )
			{
				return "";
			}
			if ( errors.size() == 1 )
			{
				return errors[0];
			}

			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();

			builder << errors[0];
			for ( size_t i = 1; i < errors.size(); ++i )
			{
				builder << ", " << errors[i];
			}

			return lease.toString();
		}
	}

	//=====================================================================
	// Base Vista exception
	//=====================================================================

	VistaException::VistaException( const std::string& message )
		: std::runtime_error{ "VistaException - " + message }
	{
	}

	//=====================================================================
	// Validation exception
	//=====================================================================

	ValidationException::ValidationException( const std::string& message )
		: VistaException{ "Validation failed - Message='" + message + "'" }
	{
	}

	ValidationException::ValidationException( const ValidateResult::Invalid& result )
		: VistaException{ "Validation failed - Message='[" + internal::joinErrors( result.errors() ) + "]'" }
	{
	}
}
