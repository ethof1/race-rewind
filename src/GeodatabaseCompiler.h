#include "Forward.h"

// TODO: Move this to the cpp file?
#include <FileGDBAPI.h>

namespace rrewind
{
	/**
	 * Compiles a geodatabase from telemetry data provided to this class. 
	 */
	class GeodatabaseCompiler
	{
	public:
		/**
		 * Constructor
		 *
		 * @param folderPath The folder location of where to create the geodatabase. The provided 
		 *		path should inlcude the ".gdb" suffix
		 */
		GeodatabaseCompiler(const std::string &folderPath);

		/**
		 * Temporary function to add test entries into the geodatabase managed by this instance.
		 */
		void addTelemetryEntries();

	private:
		/**
		 * Creates the geodatabase on disk at the provided location.
		 * 
		 * @return True if the geodatabase was successfully created on disk, false otherwise
		 */
		bool createGeodatabase();

		/**
		 * Creates the geodatabase schema for telemetry data.
		 *
		 * @return True if the schema was successfully created, false otherwise
		 */
		bool createSchema();

	private:
		std::string mFolderPath;
		std::unique_ptr<FileGDBAPI::Geodatabase> mGeodatabase;
		FileGDBAPI::Table mTelemetryTable;
	};
}