/**
 * @file DateTime.h
 * @brief Vista SDK date and time type aliases
 * @details Provides convenient type aliases for nfx::time date and time types.
 */

#pragma once

#include <nfx/DateTime.h>

namespace dnv::vista::sdk
{
    /** @brief Date and time without timezone information */
    using DateTime = nfx::time::DateTime;

    /** @brief Date and time with timezone offset */
    using DateTimeOffset = nfx::time::DateTimeOffset;

    /** @brief Time interval or duration */
    using TimeSpan = nfx::time::TimeSpan;
} // namespace dnv::vista::sdk
