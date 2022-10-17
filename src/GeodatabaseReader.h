#include "Forward.h"

#include <FileGDBAPI.h>

namespace rrewind
{
	/**
	 * Temporary container for entries from the geodatabase.
	 */
	struct TelemetryEntry
	{
		std::string mDriverId;
		std::int32_t mTimeOffset;
		double mLat;
		double mLon;
	};

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