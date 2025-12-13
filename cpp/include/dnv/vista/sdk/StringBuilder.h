/**
 * @file StringBuilder.h
 * @brief Type alias for nfx::string::StringBuilder
 * @details Provides StringBuilder in dnv::vista::sdk namespace as an alias to nfx::string::StringBuilder
 */

#pragma once

#include <nfx/string/StringBuilder.h>

namespace dnv::vista::sdk
{
    /**
     * @brief High-performance string builder with efficient memory management
     * @details Type alias for nfx::string::StringBuilder. See nfx::string::StringBuilder documentation
     *          for full API reference.
     */
    using StringBuilder = nfx::string::StringBuilder;
} // namespace dnv::vista::sdk
