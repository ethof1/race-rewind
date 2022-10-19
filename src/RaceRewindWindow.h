#ifndef RREWIND_RACEREWINDWINDOW_H
#define RREWIND_RACEREWINDWINDOW_H

#include "Forward.h"

#include "GeodatabaseReader.h"

#include <GraphicsOverlay.h>
#include <Graphic.h>

#include <QMainWindow>

#include <map>

namespace rrewind
{
	/**
	 * The main window class for the Race Rewind application.
	 */
	class RaceRewindWindow : public QMainWindow
	{
	public:
		/**
		 * Create the application window.
		 * 
		 * @param parent The parent of this window
		 */
		RaceRewindWindow(QWidget* parent = nullptr);

	private:
		/**
		 * Create all UI components for this widget.
		 */
		void setupUi();

		/**
		 * Handle the action to compile a telemetry geodatabase.
		 */
		void handleCompileGdb();

		/**
		 * Handle the action to open a telemetry geodatabase and being displaying contents.
		 */
		void handleOpenGdb();

		/**
		 * Handle changes to the current session time by updating the telemetry data shown
		 * to the specified timestamp.
		 *
		 * @param newTimestamp The timestamp at which to show telemetry data
		 */
		void handleTimeChanged(int newTimestamp);

		/**
		 * Update the map graphic locations for all drivers based on the provided telemetry data.
		 * 
		 * @param driverTelemetryMap A map from driver ID to its telemetry data for a point in time
		 */
		void updateGraphicLocations(const std::map<std::string, TelemetryEntry> &driverTelemetryMap);

	private:
		Esri::ArcGISRuntime::GraphicsOverlay *mGraphicsOverlay;
		std::map<std::string, Esri::ArcGISRuntime::Graphic *> mDriverGraphicMap;

		// Use a nullptr as a sentinel for the reader being in an invalid state
		std::unique_ptr<GeodatabaseReader> mGdbReader;
	};
}

#endif