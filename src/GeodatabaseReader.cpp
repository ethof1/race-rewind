#include "GeodatabaseReader.h"

#include "StringUtils.h"

#include <QtDebug>

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

	GeodatabaseReader::GeodatabaseReader(const std::string &gdbPath) :
		mGeodatabase(new FileGDBAPI::Geodatabase())
	{
		if (!openGeodatabase(gdbPath))
		{
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

		return success;
	}

	std::vector<TelemetryEntry> GeodatabaseReader::getPointFeatures()
	{
		if (!mGeodatabase)
		{
			return {};
		}

		std::vector<TelemetryEntry> results;

		FileGDBAPI::Table telemetryTable;
		fgdbError errorCode = mGeodatabase->OpenTable(L"\\TrackPosition", telemetryTable);
		if (errorCode != S_OK)
		{
			qCritical() << "An error occurred while opening the geodatabase: " << getErrorStr(errorCode);
			return {};
		}

		FileGDBAPI::EnumRows rows;
		errorCode = telemetryTable.Search(L"*", L"", false, rows);
		if (errorCode != S_OK)
		{
			qCritical() << "An error occurred while retrieving query results: " << getErrorStr(errorCode);
			return {};
		}

		FileGDBAPI::Row row;
		std::wstring driverId;
		FileGDBAPI::PointShapeBuffer shapeBuffer;
		FileGDBAPI::Point *point;
		std::int32_t timeOffset;

		while (rows.Next(row) == S_OK)
		{
			row.GetString(L"DRIVER_ID", driverId);

			row.GetGeometry(shapeBuffer);
			shapeBuffer.GetPoint(point);

			row.GetInteger(L"TIMESTAMP", timeOffset);

			TelemetryEntry entry;
			entry.mDriverId = std::string(driverId.begin(), driverId.end());
			entry.mTimeOffset = timeOffset;
			entry.mLon = point->x;
			entry.mLat = point->y;

			results.push_back(entry);
		}

		return results;
	}
}