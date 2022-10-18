#ifndef RREWIND_FORWARD_H
#define RREWIND_FORWARD_H

#include "Forward.h"

namespace rrewind
{
	struct TelemetryEntry
	{
		std::string mDriverId;
		std::int32_t mTimeOffset;
		double mLat;
		double mLon;
	};
}

#endif