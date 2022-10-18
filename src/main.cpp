#include "EsriUtils.h"
#include "RaceRewindWindow.h"

#include "GeodatabaseCompiler.h"

#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Initialize the Esri SDK
    rrewind::configureEsriApi();

    // rrewind::GeodatabaseCompiler compiler("c:/users/etho/desktop/mygdb.gdb");
    // compiler.addTelemetryEntries();

    // Create the main window
    rrewind::RaceRewindWindow *mainWindow = new rrewind::RaceRewindWindow(nullptr);
    mainWindow->setWindowTitle("Race Rewind");
    mainWindow->resize(800, 600);
    mainWindow->show();

    // Start the main loop
    return app.exec();
}
