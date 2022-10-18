#ifndef RREWIND_RACEREWINDWINDOW_H
#define RREWIND_RACEREWINDWINDOW_H

#include "Forward.h"

#include "GeodatabaseReader.h"

#include <GraphicsOverlay.h>

#include <QMainWindow>

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

		void handleTimeChanged(int newValue);

	private:
		Esri::ArcGISRuntime::GraphicsOverlay *mGraphicsOverlay;

		std::unique_ptr<GeodatabaseReader> mGdbReader;
	};
}

#endif