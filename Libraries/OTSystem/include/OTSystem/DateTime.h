//! @file DateTime.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/SystemAPIExport.h"

// std header
#include <string>

namespace ot {

    //! @brief Convenience methods for date and time operations.
    class OT_SYS_API_EXPORT DateTime {
    public:

        //! @brief Date format.
        enum DateFormat {
            Simple,     //! @brief Simple format, current timezone ("yyyy-mm-dd hh:MM:ss.zzz", e.g. "2000-01-01 00:00:00.000").
            SimpleUTC,  //! @brief Simple format, UTC time ("yyyy-mm-dd hh:MM:ss.zzz", e.g. "2000-01-01 00:00:00.000").
            ISO8601UTC, //! @brief ISO 8601, UTC time ("yyyy-mm-ddThh:MM:ss.zzzZ", e.g. "2000-01-01T00:00:00.000Z").
            RFC3339     //! @brief ISO 8601, explicit timezone ("yyyy-mm-ddThh:MM:ss.zzzZ", e.g. "2000-01-01T00:00:00.000+00:00").
        };

        //! @brief Returns the current time as a string with the given format.
        static std::string currentTimestamp(DateFormat _format);

        //! @brief Current milliseconds since epoch.
        static long long getMSSinceEpoc(void);

    };

}