#ifndef RREWIND_TELEMETRYCACHELOADER_H
#define RREWIND_TELEMETRYCACHELOADER_H

#include "Forward.h"

#include "LocalSpaceConverter.h"
#include "TelemetryEntry.h"

#include <QFile>

namespace rrewind
{
	/**
	 * This class handles pulling race session telemetry data from the internet, and making it available 
	 * in a structured form.
	 * 
	 * Callers should use the following pattern after constructing an instance of this class:
	 * - pullTelemetryCache()
	 * - startCacheRead()
	 * - hasNextEntry()
	 * - getNextEntry()
	 */
	class TelemetryCacheLoader
	{
	public:
		/**
		 * Construct a telemetry cache loader
		 */
		TelemetryCacheLoader();

		/**
		 * Pull telemetry data from the internet, and store in a local CSV cache.
		 *
		 * @return True if the cache pull was successful, otherwise false.
		 */
		bool pullTelemetryCache();

		/**
		 * Open the cache CSV file for reading.
		 * 
		 * @return True if the file could be opened, otherwise false
		 */
		bool startCacheRead();

		/**
		 * Check whether at least one more telemetry entry is available to be read.
		 *
		 * @return True if additional entries are available, otherwise false
		 */
		bool hasNextEntry() const;

		/**
		 * Get the next telemetry entry in the cache.
		 *
		 * @todo Use std::optional if the next entry could not be read.
		 *
		 * @return The next telemetry entry from the cache
		 */
		TelemetryEntry getNextEntry();

	private:
		QFile mCacheFile;
		LocalSpaceConverter mPtConverter;

		qint64 mBaseEpochMsecs;
	};
}

#endif