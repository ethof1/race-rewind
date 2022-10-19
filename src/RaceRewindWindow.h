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
		 * Constructor
		 * 
		 * @param parent The parent of this window
		 */
		RaceRewindWindow(QWidget* parent = nullptr);

	private:
		/**
		 * Create all UI components for this widget.
		 */
		void setupUi();

		void handleCompileGdb();

		void handleOpenGdb();

		void handleTimeChanged(int newValue);

		void updateGraphicLocations(const std::map<std::string, TelemetryEntry> &driverTelemetryMap);

	private:
		Esri::ArcGISRuntime::GraphicsOverlay *mGraphicsOverlay;
		std::map<std::string, Esri::ArcGISRuntime::Graphic *> mDriverGraphicMap;

		std::unique_ptr<GeodatabaseReader> mGdbReader;
	};
}

#endif