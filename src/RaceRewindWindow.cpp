#include "RaceRewindWindow.h"

#include "GeodatabaseCompiler.h"
#include "GeodatabaseReader.h"
#include "TelemetryCacheLoader.h"
#include "TelemetryEntry.h"

#include <Basemap.h>
#include <CompositeSymbol.h>
#include <MapGraphicsView.h>
#include <Map.h>
#include <PolylineBuilder.h>
#include <PolygonBuilder.h>
#include <SimpleFillSymbol.h>
#include <SimpleLabelExpression.h>
#include <SimpleMarkerSymbol.h>
#include <TextSymbol.h>

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
        /**
         * Container for the map graphics style for each driver.
         */
        struct GraphicsConfig
        {
            std::string mDriverId;
            Qt::GlobalColor mFillColor;
            Qt::GlobalColor mLineColor;
        };
    }

	RaceRewindWindow::RaceRewindWindow(QWidget *parent) :
        QMainWindow(parent),
        mGraphicsOverlay(nullptr)
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

        // Create a slider for adjusting the current session time
        QDockWidget *bottomDockPanel = new QDockWidget(tr("Session Time"), this);
        bottomDockPanel->setAllowedAreas(Qt::BottomDockWidgetArea);
        this->addDockWidget(Qt::BottomDockWidgetArea, bottomDockPanel);

        QSlider *sessionTimeSlider = new QSlider(Qt::Horizontal, this);
        sessionTimeSlider->setRange(0, 600000); // 10 minutes
        sessionTimeSlider->setTickInterval(10000); // 10 seconds
        sessionTimeSlider->setSingleStep(5000); // 5 seconds
        sessionTimeSlider->setTickPosition(QSlider::TicksBelow);
        bottomDockPanel->setWidget(sessionTimeSlider);

        connect(sessionTimeSlider, &QSlider::valueChanged, this, &RaceRewindWindow::handleTimeChanged);

        // Create menu entries
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

        mGraphicsOverlay = new Esri::ArcGISRuntime::GraphicsOverlay(this);

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
        // Ask the user where the geodatabase is located on disk
        QString gdbPath = QFileDialog::getExistingDirectory(this, "Select Geodatabase", ".");

        if (QFileInfo(gdbPath).exists())
        {
            // Open the geodatabase
            mGdbReader = std::make_unique<GeodatabaseReader>(gdbPath.toStdString());

            if (mGdbReader)
            {
                // TODO: The graphics could be configured earlier, during the UI setup phase. Leaving
                // this here likely will cause issues if multiple geodatabases are opened in the same application session

                // Setup the point graphic style for each driver. Use team colors
                // TODO: Load from a config file
                std::vector<GraphicsConfig> graphicsConfig;
                graphicsConfig.emplace_back("VER", Qt::blue, Qt::black);
                graphicsConfig.emplace_back("PER", Qt::blue, Qt::yellow);
                graphicsConfig.emplace_back("RUS", Qt::cyan, Qt::black);
                graphicsConfig.emplace_back("HAM", Qt::cyan, Qt::yellow);
                graphicsConfig.emplace_back("LEC", Qt::red, Qt::black);
                graphicsConfig.emplace_back("SAI", Qt::red, Qt::yellow);
                graphicsConfig.emplace_back("RIC", Qt::darkYellow, Qt::black);
                graphicsConfig.emplace_back("NOR", Qt::darkYellow, Qt::yellow);
                graphicsConfig.emplace_back("ALO", Qt::magenta, Qt::black);
                graphicsConfig.emplace_back("OCO", Qt::magenta, Qt::yellow);
                graphicsConfig.emplace_back("VET", Qt::darkGreen, Qt::black);
                graphicsConfig.emplace_back("STR", Qt::darkGreen, Qt::yellow);
                graphicsConfig.emplace_back("GAS", Qt::darkBlue, Qt::black);
                graphicsConfig.emplace_back("TSU", Qt::white, Qt::yellow);
                graphicsConfig.emplace_back("BOT", Qt::darkRed, Qt::black);
                graphicsConfig.emplace_back("ZHO", Qt::darkRed, Qt::yellow);
                graphicsConfig.emplace_back("MAG", Qt::gray, Qt::black);
                graphicsConfig.emplace_back("MSC", Qt::gray, Qt::yellow);
                graphicsConfig.emplace_back("DEV", Qt::darkGray, Qt::black);
                graphicsConfig.emplace_back("LAT", Qt::darkGray, Qt::yellow);

                mGraphicsOverlay->setLabelsEnabled(true);

                for (const auto &config : graphicsConfig)
                {
                    // Set point graphic style
                    Esri::ArcGISRuntime::SimpleLineSymbol *pointOutline = new Esri::ArcGISRuntime::SimpleLineSymbol(
                        Esri::ArcGISRuntime::SimpleLineSymbolStyle::Solid, QColor(config.mLineColor), 2, this);

                    Esri::ArcGISRuntime::SimpleMarkerSymbol *pointSymbol = new Esri::ArcGISRuntime::SimpleMarkerSymbol(
                        Esri::ArcGISRuntime::SimpleMarkerSymbolStyle::Circle, QColor(config.mFillColor), 10, this);

                    pointSymbol->setOutline(pointOutline);

                    // Set text label for driver name
                    Esri::ArcGISRuntime::TextSymbol *textSymbol = new Esri::ArcGISRuntime::TextSymbol(
                        config.mDriverId.c_str(), QColor(Qt::black), 12.0, Esri::ArcGISRuntime::HorizontalAlignment::Right, 
                        Esri::ArcGISRuntime::VerticalAlignment::Middle, this);
                    
                    textSymbol->setHaloColor(Qt::white);
                    textSymbol->setHaloWidth(3);
                    textSymbol->setOffsetX(-10);
                    textSymbol->setOffsetY(5);

                    // Create a composite symbol that includes the point and the label
                    QList<Esri::ArcGISRuntime::Symbol *> symbols;
                    symbols.append(pointSymbol);
                    symbols.append(textSymbol);

                    Esri::ArcGISRuntime::CompositeSymbol *compositeSymbol = new Esri::ArcGISRuntime::CompositeSymbol(symbols, this);

                    // Initialize the point to (0,0) until the first update is received
                    Esri::ArcGISRuntime::Point pointLoc(0, 0, Esri::ArcGISRuntime::SpatialReference::wgs84());
                    Esri::ArcGISRuntime::Graphic *pointGraphic = new Esri::ArcGISRuntime::Graphic(pointLoc, compositeSymbol, this);
                    
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

    void RaceRewindWindow::handleTimeChanged(int newTimestamp)
    {
        if (mGdbReader)
        {
            updateGraphicLocations(mGdbReader->getEntriesAtTimestamp(newTimestamp));
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
                // to avoid being out-of-sync for large jumps in time
                driverGraphic.second->setVisible(false);
            }
        }
    }
}