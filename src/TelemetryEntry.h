#ifndef RREWIND_TELEMETRYENTRY_H
#define RREWIND_TELEMETRYENTRY_H

#include "Forward.h"

namespace rrewind
{
	/**
	 * Container for race session telemetry data.
	 * 
	 * @todo Expand to include other data captured for each timestamp
	 */
	struct TelemetryEntry
	{
		std::string mDriverId; ///< ID of the driver this telemetry data applies to
		std::int32_t mTimeOffset; ///< The time offset (from session start) at which this telemetry data was captured/relevant
		double mLat; ///< The geodetic latitude component of the driver's location
		double mLon; ///< The geodetic longitude component of the driver's location
	};
}

#endif