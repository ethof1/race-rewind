#include "TelemetryCacheLoader.h"

#include <QFile>
#include <QProcess>
#include <QtDebug>

namespace rrewind
{
	namespace
	{
		const std::string PYTHON_PATH = "C:/Users/etho/Downloads/python-3.10.8-embed-amd64";
		const std::string PYTHON_EXE = "python.exe";
		const std::string PYTHON_SCRIPT = "f1test.py";
		const std::string CACHE_OUTPUT_FILE = "f1.csv";
	}

	TelemetryCacheLoader::TelemetryCacheLoader()
	{
		// Do nothing
	}

	bool TelemetryCacheLoader::pullTelemetryCache()
	{
		// TODO: Clear the cache file

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

		if (!process.waitForFinished())
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
		if (mCacheFile.open(QIODevice::ReadOnly))
		{
			// The first line is a header line, so read past it
			mCacheFile.readLine();
		}
		else
		{
			qCritical() << "Could not open cache file for reading: " << mCacheFile.errorString();
			success = false;
		}

		return success;
	}

	bool TelemetryCacheLoader::hasNextEntry()
	{
		return mCacheFile.isOpen() && !mCacheFile.atEnd();
	}

	TelemetryEntry TelemetryCacheLoader::getNextEntry()
	{
		TelemetryEntry entry;
		QStringList lineTokens;
		
		if (!mCacheFile.atEnd()) {
			QByteArray line = mCacheFile.readLine();
			for (const auto &token : line.split(','))
			{
				lineTokens.append(token);
			}
		}

		// Ignore the line if it is not the expected size
		if (lineTokens.size() == 9)
		{
			entry.mDriverId = "VER";

			auto convertedPts = mPtConverter.convert(lineTokens.at(3).toDouble(), lineTokens.at(4).toDouble());
			entry.mLon = convertedPts.first;
			entry.mLat = convertedPts.second;

			// TODO: Read and convert from file
			entry.mTimeOffset = 10;
		}

		return entry;
	}
}