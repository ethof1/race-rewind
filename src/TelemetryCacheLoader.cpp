#include "TelemetryCacheLoader.h"

#include <QFile>
#include <QDateTime>
#include <QProcess>
#include <QtDebug>

namespace rrewind
{
	namespace
	{
		// TODO: Remove hardcoded absolute path
		const std::string PYTHON_PATH = "C:/Users/etho/Downloads/python-3.10.8-embed-amd64";
		const std::string PYTHON_EXE = "python.exe";
		const std::string PYTHON_SCRIPT = "f1test.py";
		const std::string CACHE_OUTPUT_FILE = "f1.csv";
	}

	TelemetryCacheLoader::TelemetryCacheLoader() :
		mBaseEpochMsecs(0)
	{
		// Do nothing
	}

	bool TelemetryCacheLoader::pullTelemetryCache()
	{
		// Run the python script to pull telemetry data from the internet and cache locally to a CSV file
		QProcess process;
		process.setWorkingDirectory(PYTHON_PATH.c_str());
		process.setProcessChannelMode(QProcess::MergedChannels);

		QStringList args;
		args << PYTHON_SCRIPT.c_str();

		process.start((PYTHON_PATH + "/" + PYTHON_EXE).c_str(), args);

		if (!process.waitForStarted())
		{
			qCritical() << "Could not start python: " << process.errorString();
			return false;
		}

		// To be safe, timeout after 10 minutes. In addition to the time required to pull the remote cache, for a full 
		// race with ~0.5 second sampling for 20 drivers, there will be on the order of a million samples to process
		if (!process.waitForFinished(10 * 60000)) 
		{
			qCritical() << "Python script did not complete";
			return false;
		}

		return true;
	}

	bool TelemetryCacheLoader::startCacheRead()
	{
		bool success = true;

		// Load the cache CSV file
		mCacheFile.setFileName((PYTHON_PATH + "/" + CACHE_OUTPUT_FILE).c_str());
		if (!mCacheFile.open(QIODevice::ReadOnly))
		{
			qCritical() << "Could not open cache file for reading: " << mCacheFile.errorString();
			success = false;
		}

		return success;
	}

	bool TelemetryCacheLoader::hasNextEntry() const
	{
		return mCacheFile.isOpen() && !mCacheFile.atEnd();
	}

	TelemetryEntry TelemetryCacheLoader::getNextEntry()
	{
		TelemetryEntry entry;
		QStringList lineTokens;
		
		// Tokenize the line at the delimiters
		if (!mCacheFile.atEnd())
		{
			QByteArray line = mCacheFile.readLine();
			for (const auto &token : line.split(','))
			{
				lineTokens.append(token);
			}
		}

		// Ignore the line if it is not the expected size
		if (lineTokens.size() == 10)
		{
			entry.mDriverId = lineTokens.at(9).trimmed().toStdString();

			auto convertedPts = mPtConverter.convert(lineTokens.at(3).toDouble(), lineTokens.at(4).toDouble());
			entry.mLon = convertedPts.first;
			entry.mLat = convertedPts.second;

			// Parse the absolute timestamp and convert to an offset from the first time entry
			QDateTime timestamp = QDateTime::fromString(lineTokens.at(1), Qt::ISODateWithMs);
			if (timestamp.isValid())
			{
				qint64 epochMsecs = timestamp.toMSecsSinceEpoch();
				
				// Set a base timestamp if this is the first entry
				if (mBaseEpochMsecs == 0)
				{
					mBaseEpochMsecs = epochMsecs;
				}

				qint64 timeOffset = epochMsecs - mBaseEpochMsecs;
				
				// The timeOffset range should be within 32 bits, so this conversion is safe
				entry.mTimeOffset = static_cast<std::int32_t>(timeOffset);
			}
		}

		return entry;
	}
}