#ifndef RREWIND_GEODATABASEREADER_H
#define RREWIND_GEODATABASEREADER_H

#include "Forward.h"

#include "TelemetryEntry.h"

#include <FileGDBAPI.h>

namespace rrewind
{
	/**
	 * A reader of telemetry geodatabase content.
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
		 * Retrieve point features from the geodatabase.
		 * 
		 * @return A list of all telemetry point features
		 */
		std::vector<TelemetryEntry> getPointFeatures();

	private:
		/**
		 * Open the geodatbase for reading.
		 * 
		 * @param gdbPath The path to the geodatabase to open
		 */
		bool openGeodatabase(const std::string &gdbPath);

	private:
		std::unique_ptr<FileGDBAPI::Geodatabase> mGeodatabase;
	};
}

#endif