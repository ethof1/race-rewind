#include "RaceRewindWindow.h"

#include "GeodatabaseCompiler.h"
#include "GeodatabaseReader.h"
#include "TelemetryCacheLoader.h"
#include "TelemetryEntry.h"

#include <Basemap.h>
#include <MapGraphicsView.h>
#include <Map.h>
#include <PolylineBuilder.h>
#include <PolygonBuilder.h>
#include <SimpleFillSymbol.h>
#include <SimpleMarkerSymbol.h>

#include <QDockWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QProgressDialog>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

#include <QtConcurrent>

namespace rrewind
{
    namespace
    {
        struct GraphicsConfig
        {
            std::string mDriverId;
            Qt::GlobalColor mFillColor;
            Qt::GlobalColor mLineColor;
        };
    }

	RaceRewindWindow::RaceRewindWindow(QWidget *parent) :
        QMainWindow(parent)
	{
		setupUi();
	}

	void RaceRewindWindow::setupUi()
	{
        Esri::ArcGISRuntime::MapGraphicsView *mapView = new Esri::ArcGISRuntime::MapGraphicsView(nullptr, this);

        // Create a basemap and set it on the MapView
        Esri::ArcGISRuntime::Basemap *basemap = new Esri::ArcGISRuntime::Basemap(Esri::ArcGISRuntime::BasemapStyle::ArcGISImagery, this);
        Esri::ArcGISRuntime::Map *map = new Esri::ArcGISRuntime::Map(basemap, this);
        mapView->setMap(map);

        this->setCentralWidget(mapView);

        // =================

        QDockWidget *bottomDockPanel = new QDockWidget(tr("Session Time"), this);
        bottomDockPanel->setAllowedAreas(Qt::BottomDockWidgetArea);
        this->addDockWidget(Qt::BottomDockWidgetArea, bottomDockPanel);

        QSlider *sessionTimeSlider = new QSlider(Qt::Horizontal, this);
        sessionTimeSlider->setRange(0, 600000);
        sessionTimeSlider->setTickInterval(10000);
        sessionTimeSlider->setSingleStep(5000);
        sessionTimeSlider->setTickPosition(QSlider::TicksBelow);
        bottomDockPanel->setWidget(sessionTimeSlider);

        connect(sessionTimeSlider, &QSlider::valueChanged, this, &RaceRewindWindow::handleTimeChanged);

        QMenu *fileMenu = this->menuBar()->addMenu("&File");

        QAction *compileGdbAction = new QAction("Compile GDB...");
        connect(compileGdbAction, &QAction::triggered, this, &RaceRewindWindow::handleCompileGdb);

        QAction *openAction = new QAction("Open...");
        connect(openAction, &QAction::triggered, this, &RaceRewindWindow::handleOpenGdb);

        QAction *exitAction = new QAction("Exit");
        connect(exitAction, &QAction::triggered, this, &QWidget::close);

        fileMenu->addAction(compileGdbAction);
        fileMenu->addAction(openAction);
        fileMenu->addSeparator();
        fileMenu->addAction(exitAction);

        // =================
       
        // TODO: Remove hardcode
        
        // std::vector<TelemetryEntry> results = reader.getPointFeatures();

        // qInfo() << "Found " << results.size() << " results";

        // Test display of a point on the map (taken from SDK example)
        mGraphicsOverlay = new Esri::ArcGISRuntime::GraphicsOverlay(this);

        

        // Create a graphic to display the point with its symbology
 //       for (const auto &entry : results)
        

        mapView->graphicsOverlays()->append(mGraphicsOverlay);
	}

    void RaceRewindWindow::handleCompileGdb()
    {
        // Ask the user where to save the Geodatabase
        QString gdbPath = QFileDialog::getExistingDirectory(this, "Geodatabase Location", ".");
        
        // Show an indeterminate progress dialog while compiling
        QProgressDialog progress(this);
        progress.setWindowTitle("Compile Geodatabase");
        progress.setWindowFlag(Qt::Popup);
        progress.setLabelText("Compiling Telemetry Geodatabase...");
        progress.setRange(0, 0);

        // Setup a watcher to know when to close the progress dialog
        QFutureWatcher<void> watcher;
        connect(&watcher, &QFutureWatcher<int>::finished, &progress, &QProgressDialog::cancel);

        // Start the Geodatabase compilation in a separate thread so the UI thread is not blocked
        QFuture<void> future = QtConcurrent::run([=]() 
        {
            qInfo() << "Starting cache pull and read";

            GeodatabaseCompiler compiler(gdbPath.toStdString());
            compiler.startInserts();

            TelemetryCacheLoader cacheLoader;
            if (cacheLoader.pullTelemetryCache() && cacheLoader.startCacheRead())
            {
                // Read each entry from the cache and store in the geodatabase. Since the number
                // of telemetry entries is large, avoid loading all from the cache first, to keep memory usage low
                while (cacheLoader.hasNextEntry())
                {
                    TelemetryEntry entry = cacheLoader.getNextEntry();
                    compiler.addTelemetryEntry(entry);
                }
            }
            else
            {
                qCritical() << "Could not pull or read telemetry cache";
            }

            compiler.endInserts();
            compiler.close();

            qInfo() << "Finished cache pull and read";
        });

        watcher.setFuture(future);
        progress.exec();
    }

    void RaceRewindWindow::handleOpenGdb()
    {
        // Ask the user where to save the Geodatabase
        QString gdbPath = QFileDialog::getExistingDirectory(this, "Select Geodatabase", ".");

        if (QFileInfo(gdbPath).exists())
        {
            mGdbReader = std::make_unique<GeodatabaseReader>(gdbPath.toStdString());

            if (mGdbReader)
            {
                // Setup the point graphic style for each driver
                // TODO: Load from a config file
                std::vector<GraphicsConfig> graphicsConfig;
                graphicsConfig.emplace_back("VER", Qt::blue, Qt::black);
                graphicsConfig.emplace_back("PER", Qt::blue, Qt::yellow);

                for (const auto &config : graphicsConfig)
                {
                    Esri::ArcGISRuntime::SimpleLineSymbol *pointOutline = new Esri::ArcGISRuntime::SimpleLineSymbol(
                        Esri::ArcGISRuntime::SimpleLineSymbolStyle::Solid, QColor(config.mLineColor), 2, this);

                    Esri::ArcGISRuntime::SimpleMarkerSymbol *pointSymbol = new Esri::ArcGISRuntime::SimpleMarkerSymbol(
                        Esri::ArcGISRuntime::SimpleMarkerSymbolStyle::Circle, QColor(config.mFillColor), 10, this);

                    pointSymbol->setOutline(pointOutline);

                    // Esri::ArcGISRuntime::Point pointLoc(entry.mLon, entry.mLat, Esri::ArcGISRuntime::SpatialReference::wgs84());
                    Esri::ArcGISRuntime::Point pointLoc(0, 0, Esri::ArcGISRuntime::SpatialReference::wgs84());
                    Esri::ArcGISRuntime::Graphic *pointGraphic = new Esri::ArcGISRuntime::Graphic(pointLoc, pointSymbol, this);
                    
                    // Hide until the first update is received
                    pointGraphic->setVisible(false);
                    
                    mGraphicsOverlay->graphics()->append(pointGraphic);
                    
                    // Store with an index for easy retrieval upon update
                    mDriverGraphicMap.insert_or_assign(config.mDriverId, pointGraphic);
                }

                // Set the initial graphic locations
                updateGraphicLocations(mGdbReader->getEntriesAtTimestamp(0));
            }
        }
    }

    void RaceRewindWindow::handleTimeChanged(int newValue)
    {
        if (mGdbReader)
        {
            updateGraphicLocations(mGdbReader->getEntriesAtTimestamp(newValue));
        }
    }

    void RaceRewindWindow::updateGraphicLocations(const std::map<std::string, TelemetryEntry> &driverTelemetryMap)
    {
        for (auto driverGraphic : mDriverGraphicMap)
        {
            auto driverTelemEntry = driverTelemetryMap.find(driverGraphic.first);
            if (driverTelemEntry != driverTelemetryMap.end())
            {
                auto geometry = driverGraphic.second->geometry();

                Esri::ArcGISRuntime::Point newPoint(driverTelemEntry->second.mLon,
                    driverTelemEntry->second.mLat, Esri::ArcGISRuntime::SpatialReference::wgs84());

                driverGraphic.second->setGeometry(newPoint);
                driverGraphic.second->setVisible(true);
            }
            else
            {
                // This driver isn't included in the provided update. The graphic should be hidden
                // to avoid being out-of-sync
                driverGraphic.second->setVisible(false);
            }
        }
    }
}