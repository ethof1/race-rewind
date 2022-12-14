#include "EsriUtils.h"
#include "RaceRewindWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Initialize the Esri SDK
    rrewind::configureEsriApi();

    // Create the main window
    rrewind::RaceRewindWindow *mainWindow = new rrewind::RaceRewindWindow(nullptr);
    mainWindow->setWindowTitle("Race Rewind");
    mainWindow->resize(800, 600);
    mainWindow->show();

    // The top-level widget is intentionally not being deleted manually, as it may not be safe to do so

    // Start the main loop
    return app.exec();
}
