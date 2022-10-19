#ifndef RREWIND_GEODATABASECOMPILER_H
#define RREWIND_GEODATABASECOMPILER_H

#include "Forward.h"

#include <FileGDBAPI.h>

namespace rrewind
{
	/**
	 * Compiles a geodatabase from telemetry data provided to this class.
	 * 
	 * Callers should use the following pattern after constructing an instance of this class:
	 * - startInserts()
	 * - addTelemetryEntry()
	 * - endInserts()
	 * - close()
	 */
	class GeodatabaseCompiler
	{
	public:
		/**
		 * Create a new geodatabase at the given path. The directory should be empty.
		 *
		 * @param folderPath The folder location of where to create the geodatabase. The provided 
		 *		path should inlcude the ".gdb" suffix
		 */
		GeodatabaseCompiler(const std::string &folderPath);

		/**
		 * Lock the geodatabase for writing. This function must be called before any calls to addTelemetryEntry, 
		 * otherwise the behavior of this class is undefined.
		 * 
		 * @todo A better scoping mechanism is needed to reduce the burden on the caller, and have the start/end 
		 *		operations called automatically.
		 */
		void startInserts();

		/**
		 * Unlock the geodatabase for writing. If any calls are made to addTelemetryEntry after calling this function,
		 * the behavior is undefined.
		 * 
		 * @todo A better scoping mechanism is needed to reduce the burden on the caller, and have the start/end 
		 *		operations called automatically.
		 */
		void endInserts();

		/**
		 * Closes the geodatabase for all operations. If any additional calls are made to any functions in this class,
		 * the behavior is undefined.
		 * 
		 * @todo This should be handled in the destructor.
		 */
		void close();

		/**
		 * Add a new telemetry entry into the geodatabase managed by this instance. A call to startInserts must be made
		 * before calling this function.
		 * 
		 * @param entry The telemetry entry to add
		 */
		void addTelemetryEntry(const TelemetryEntry &entry);

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

		// Use a NULL geodatabase instance as a sentinel for the compiler being in an invalid state
		std::unique_ptr<FileGDBAPI::Geodatabase> mGeodatabase;
		FileGDBAPI::Table mTelemetryTable;
	};
}

#endif