#ifndef RREWIND_GEODATABASEREADER_H
#define RREWIND_GEODATABASEREADER_H

#include "Forward.h"

#include "TelemetryEntry.h"

#include <FileGDBAPI.h>

#include <map>

namespace rrewind
{
	/**
	 * A reader for telemetry geodatabase content.
	 */
	class GeodatabaseReader
	{
	public:
		/**
		 * Constructor
		 * 
		 * @param gdbPath The path to the geodatabase to read from
		 */
		GeodatabaseReader(const std::string &gdbPath);

		/**
		 * Retrieve telemetry entries for all available drivers at the timestamp closest to
		 * a target timestamp, but not after.
		 *
		 * @param timestamp An offset from the session start time, in milliseconds
		 * @return A map indexed by driver ID, associated with the telemetry entry closest to the @p timestamp
		 */
		std::map<std::string, TelemetryEntry> getEntriesAtTimestamp(std::int32_t timestamp);

	private:
		/**
		 * Open the geodatbase for reading.
		 * 
		 * @param gdbPath The path to the geodatabase to open
		 */
		bool openGeodatabase(const std::string &gdbPath);

	private:
		// Use a nullptr as a sentinel for the reader being in an invalid state
		std::unique_ptr<FileGDBAPI::Geodatabase> mGeodatabase;
		FileGDBAPI::Table mTelemetryTable;
	};
}

#endif