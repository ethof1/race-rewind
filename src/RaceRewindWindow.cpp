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
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

namespace rrewind
{
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
        sessionTimeSlider->setRange(0, 29000);
        bottomDockPanel->setWidget(sessionTimeSlider);

        connect(sessionTimeSlider, &QSlider::valueChanged, this, &RaceRewindWindow::handleTimeChanged);

        // =================
/*
        qInfo() << "Starting cache pull and read";

        GeodatabaseCompiler compiler("c:/users/etho/desktop/mygdb.gdb");
        compiler.startInserts();

        TelemetryCacheLoader cacheLoader;
        if (cacheLoader.pullTelemetryCache() && cacheLoader.startCacheRead())
        {
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
 */       
        // TODO: Remove hardcode
        mGdbReader = std::make_unique<GeodatabaseReader>("c:/users/etho/desktop/mygdb.gdb");
        // std::vector<TelemetryEntry> results = reader.getPointFeatures();

        // qInfo() << "Found " << results.size() << " results";

        // Test display of a point on the map (taken from SDK example)
        mGraphicsOverlay = new Esri::ArcGISRuntime::GraphicsOverlay(this);

        Esri::ArcGISRuntime::SimpleLineSymbol *pointOutline = new Esri::ArcGISRuntime::SimpleLineSymbol(
            Esri::ArcGISRuntime::SimpleLineSymbolStyle::Solid, QColor(Qt::blue), 3, this);
        
        Esri::ArcGISRuntime::SimpleMarkerSymbol *pointSymbol = new Esri::ArcGISRuntime::SimpleMarkerSymbol(
            Esri::ArcGISRuntime::SimpleMarkerSymbolStyle::Circle, QColor(Qt::red), 10, this);
        
        pointSymbol->setOutline(pointOutline);

        // Create a graphic to display the point with its symbology
 //       for (const auto &entry : results)
        {
            TelemetryEntry entry = mGdbReader->getEntryAtTimestamp("VER", 0);
            Esri::ArcGISRuntime::Point pointLoc(entry.mLon, entry.mLat, Esri::ArcGISRuntime::SpatialReference::wgs84());
            Esri::ArcGISRuntime::Graphic *pointGraphic = new Esri::ArcGISRuntime::Graphic(pointLoc, pointSymbol, this);
            mGraphicsOverlay->graphics()->append(pointGraphic);
        }

        mapView->graphicsOverlays()->append(mGraphicsOverlay);
	}

    void RaceRewindWindow::handleTimeChanged(int newValue)
    {
        TelemetryEntry entry = mGdbReader->getEntryAtTimestamp("VER", newValue);
        for (auto graphic = mGraphicsOverlay->graphics()->begin(); graphic != mGraphicsOverlay->graphics()->end(); ++graphic)
        {
            auto geometry = static_cast<Esri::ArcGISRuntime::Point>((*graphic)->geometry());
            Esri::ArcGISRuntime::Point newPoint(entry.mLon, entry.mLat, Esri::ArcGISRuntime::SpatialReference::wgs84());

            (*graphic)->setGeometry(newPoint);
        }
    }
}