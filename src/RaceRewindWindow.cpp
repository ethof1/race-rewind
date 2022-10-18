#include "RaceRewindWindow.h"

#include "GeodatabaseCompiler.h"
#include "GeodatabaseReader.h"
#include "TelemetryCacheLoader.h"
#include "TelemetryEntry.h"

#include <Basemap.h>
#include <GraphicsOverlay.h>
#include <MapGraphicsView.h>
#include <Map.h>
#include <PolylineBuilder.h>
#include <PolygonBuilder.h>
#include <SimpleFillSymbol.h>
#include <SimpleMarkerSymbol.h>

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
        
        // TODO: Remove hardcode
        GeodatabaseReader reader("c:/users/etho/desktop/mygdb.gdb");
        std::vector<TelemetryEntry> results = reader.getPointFeatures();

        qInfo() << "Found " << results.size() << " results";

        // Test display of a point on the map (taken from SDK example)
        Esri::ArcGISRuntime::GraphicsOverlay *overlay = new Esri::ArcGISRuntime::GraphicsOverlay(this);

        Esri::ArcGISRuntime::SimpleLineSymbol *pointOutline = new Esri::ArcGISRuntime::SimpleLineSymbol(
            Esri::ArcGISRuntime::SimpleLineSymbolStyle::Solid, QColor(Qt::blue), 3, this);
        
        Esri::ArcGISRuntime::SimpleMarkerSymbol *pointSymbol = new Esri::ArcGISRuntime::SimpleMarkerSymbol(
            Esri::ArcGISRuntime::SimpleMarkerSymbolStyle::Circle, QColor(Qt::red), 10, this);
        
        pointSymbol->setOutline(pointOutline);

        // Create a graphic to display the point with its symbology
        for (const auto &entry : results)
        {
            Esri::ArcGISRuntime::Point pointLoc(entry.mLon, entry.mLat, Esri::ArcGISRuntime::SpatialReference::wgs84());
            Esri::ArcGISRuntime::Graphic* pointGraphic = new Esri::ArcGISRuntime::Graphic(pointLoc, pointSymbol, this);
            overlay->graphics()->append(pointGraphic);
        }

        mapView->graphicsOverlays()->append(overlay);
	}
}