#include "GeodatabaseReader.h"

#include "StringUtils.h"

#include <QtDebug>

namespace rrewind
{
	namespace
	{
		/**
		 * Helper function to retrieve the error string from an error code.
		 *
		 * @todo Move to a common space
		 */
		std::wstring getErrorStr(fgdbError errorCode);
	}

	GeodatabaseReader::GeodatabaseReader(const std::string &gdbPath) :
		mGeodatabase(new FileGDBAPI::Geodatabase())
	{
		if (!openGeodatabase(gdbPath))
		{
			qCritical() << "Could not open geodatabase";
			mGeodatabase.reset();
		}
	}

	bool GeodatabaseReader::openGeodatabase(const std::string &gdbPath)
	{
		bool success = true;

		fgdbError errorCode = FileGDBAPI::OpenGeodatabase(convertToWString(gdbPath), *mGeodatabase);
		if (errorCode != S_OK)
		{
			qCritical() << "An error occurred while opening the geodatabase: " << getErrorStr(errorCode);
			success = false;
		}

		if (success)
		{
			fgdbError errorCode = mGeodatabase->OpenTable(L"\\TrackPosition", mTelemetryTable);
			if (errorCode != S_OK)
			{
				qCritical() << "An error occurred while opening the table: " << getErrorStr(errorCode);
				success = false;
			}
		}

		// TODO: Add a corresponding closeGeodatabase call
		
		return success;
	}

	std::map<std::string, TelemetryEntry> GeodatabaseReader::getEntriesAtTimestamp(std::int32_t timestamp)
	{
		std::map<std::string, TelemetryEntry> entries;

		// Since it doesn't seem an aggregation query can be performed 
		// (i.e., MAX() of timestamp less than target), search within a range
		std::wstring query = L"TIMESTAMP <= " + std::to_wstring(timestamp) + L" AND TIMESTAMP > " + std::to_wstring(timestamp - 500);

		FileGDBAPI::EnumRows rows;

		fgdbError errorCode = mTelemetryTable.Search(L"*", query, true, rows);
		if (errorCode != S_OK)
		{
			qCritical() << "An error occurred while retrieving query results: " << getErrorStr(errorCode);
			return entries;
		}

		FileGDBAPI::Row row;
		std::wstring wDriverId;
		std::string driverId;
		FileGDBAPI::PointShapeBuffer shapeBuffer;
		FileGDBAPI::Point *point;
		std::int32_t timeOffset;

		// TODO: Determine why this row iteration and processing is unexpectedly slow
		while (rows.Next(row) == S_OK)
		{
			row.GetString(L"DRIVER_ID", wDriverId);
			row.GetInteger(L"TIMESTAMP", timeOffset);

			// TODO: Use a better conversion
			driverId.assign(wDriverId.begin(), wDriverId.end());
			
			// For each driver, find the result that is closest to the target timestamp
			auto entryItr = entries.find(driverId);
			if (entryItr == entries.end() || timeOffset > entryItr->second.mTimeOffset)
			{
				row.GetGeometry(shapeBuffer);
				shapeBuffer.GetPoint(point);

				TelemetryEntry entry;
				entry.mDriverId = driverId;
				entry.mTimeOffset = timeOffset;
				entry.mLon = point->x;
				entry.mLat = point->y;

				entries.insert_or_assign(driverId, entry);
			}
		}

		return entries;
	}
}

namespace rrewind
{
	namespace
	{
		std::wstring getErrorStr(fgdbError errorCode)
		{
			std::wstring errorText;
			FileGDBAPI::ErrorInfo::GetErrorDescription(errorCode, errorText);

			return errorText;
		}
	}
}