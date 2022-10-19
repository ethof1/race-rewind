#ifndef RREWIND_TELEMETRYCACHELOADER_H
#define RREWIND_TELEMETRYCACHELOADER_H

#include "Forward.h"

#include "LocalSpaceConverter.h"
#include "TelemetryEntry.h"

#include <QFile>

namespace rrewind
{
	class TelemetryCacheLoader
	{
	public:
		TelemetryCacheLoader();

		bool pullTelemetryCache();

		bool startCacheRead();

		bool hasNextEntry();

		TelemetryEntry getNextEntry();

	private:
		QFile mCacheFile;
		LocalSpaceConverter mPtConverter;

		qint64 mBaseEpochMsecs;
	};
}

#endif