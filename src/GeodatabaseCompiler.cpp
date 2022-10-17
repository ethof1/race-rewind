#include "GeodatabaseCompiler.h"
#include "StringUtils.h"

#include <FileGDBAPI.h>

#include <QtDebug>

#include <vector>

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

	GeodatabaseCompiler::GeodatabaseCompiler(const std::string &folderPath) :
		mFolderPath(folderPath),
		mGeodatabase(new FileGDBAPI::Geodatabase())
	{
		if (createGeodatabase() && createSchema())
		{
			qInfo() << "Geodatabase and schema created successfully";
		}
		else
		{
			mGeodatabase.reset(nullptr);
		}
	}

	bool GeodatabaseCompiler::createGeodatabase()
	{
		// Create a new geodatabase in the specified directory
		fgdbError errorCode = FileGDBAPI::CreateGeodatabase(convertToWString(mFolderPath), *mGeodatabase);

        if (errorCode != S_OK)
        {
            qCritical() << "An error occurred while creating the geodatabase: " << getErrorStr(errorCode);
            return false;
        }

		return true;
	}

	bool GeodatabaseCompiler::createSchema()
	{
		if (!mGeodatabase)
		{
			return false;
		}

		bool success = true;

		// Setup the coordinate system
		FileGDBAPI::SpatialReference srid;
		srid.SetSpatialReferenceID(4326);

		// Define the geometry type
		FileGDBAPI::GeometryDef geometryDef;
		geometryDef.SetGeometryType(FileGDBAPI::geometryPoint);
		geometryDef.SetSpatialReference(srid);
		
		// Define the field types
		std::vector<FileGDBAPI::FieldDef> fieldDefinitions;

		FileGDBAPI::FieldDef driverIdField;
		driverIdField.SetName(L"DRIVER_ID");
		driverIdField.SetType(FileGDBAPI::fieldTypeString);
		driverIdField.SetIsNullable(false);
		fieldDefinitions.push_back(driverIdField);

		FileGDBAPI::FieldDef timestampField;
		timestampField.SetName(L"TIMESTAMP");
		timestampField.SetType(FileGDBAPI::fieldTypeDate);
		timestampField.SetIsNullable(false);
		fieldDefinitions.push_back(timestampField);

		FileGDBAPI::FieldDef trackPosField;
		trackPosField.SetName(L"TRACK_POS");
		trackPosField.SetType(FileGDBAPI::fieldTypeGeometry);
		trackPosField.SetIsNullable(false);
		trackPosField.SetGeometryDef(geometryDef);
		fieldDefinitions.push_back(trackPosField);

		// Create the telemetry table		
		fgdbError errorCode = mGeodatabase->CreateTable(
			L"\\TrackPosition", fieldDefinitions, L"", mTelemetryTable);
		if (errorCode != S_OK)
		{
			qCritical() << "An error occured while creating table: " << getErrorStr(errorCode);
			success = false;
		}

		return success;
	}

	void GeodatabaseCompiler::addTelemetryEntries()
	{
		if (!mGeodatabase)
		{
			return;
		}

		// Lock the table for writing
		mTelemetryTable.SetWriteLock();
		mTelemetryTable.LoadOnlyMode(true);

		// Create the row data
		FileGDBAPI::Row telemetryRow;
		mTelemetryTable.CreateRowObject(telemetryRow);

		FileGDBAPI::PointShapeBuffer shapeBuffer;
		shapeBuffer.Setup(FileGDBAPI::shapePoint);

		FileGDBAPI::Point *point;
		shapeBuffer.GetPoint(point);

		point->x = 45.0;
		point->y = 45.0;

		fgdbError errorCode = telemetryRow.SetGeometry(shapeBuffer);
		if (errorCode != S_OK)
		{
			qCritical() << "An error occurred while setting geometry: " << getErrorStr(errorCode);
		}

		errorCode = telemetryRow.SetString(L"DRIVER_ID", L"VER");
		if (errorCode != S_OK)
		{
			qCritical() << "An error occurred while setting field: " << getErrorStr(errorCode);
		}

		// Add the row to the table
		errorCode = mTelemetryTable.Insert(telemetryRow);
		if (errorCode != S_OK)
		{
			qCritical() << "An error occurred while inserting row: " << getErrorStr(errorCode);
		}

		mTelemetryTable.LoadOnlyMode(false);
		mTelemetryTable.FreeWriteLock();

		// TODO: Close the table and geodatabase when all writing is finished
		errorCode = mGeodatabase->CloseTable(mTelemetryTable);
		if (errorCode != S_OK)
		{
			qCritical() << "An error occurred while closing the table: " << getErrorStr(errorCode);
		}

		errorCode = FileGDBAPI::CloseGeodatabase(*mGeodatabase);
		if (errorCode != S_OK)
		{
			qCritical() << "An error occurred while closing the geodatabase: " << getErrorStr(errorCode);
		}
	}
}