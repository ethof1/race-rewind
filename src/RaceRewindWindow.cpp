#include "RaceRewindWindow.h"

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

        // Test display of a point on the map (taken from SDK example)
        Esri::ArcGISRuntime::GraphicsOverlay *overlay = new Esri::ArcGISRuntime::GraphicsOverlay(this);

        Esri::ArcGISRuntime::Point dumeBeach(-118.80657463861, 34.0005930608889, Esri::ArcGISRuntime::SpatialReference::wgs84());
        
        Esri::ArcGISRuntime::SimpleLineSymbol *pointOutline = new Esri::ArcGISRuntime::SimpleLineSymbol(
            Esri::ArcGISRuntime::SimpleLineSymbolStyle::Solid, QColor(Qt::blue), 3, this);
        
        Esri::ArcGISRuntime::SimpleMarkerSymbol *pointSymbol = new Esri::ArcGISRuntime::SimpleMarkerSymbol(
            Esri::ArcGISRuntime::SimpleMarkerSymbolStyle::Circle, QColor(Qt::red), 10, this);
        
        pointSymbol->setOutline(pointOutline);

        // Create a graphic to display the point with its symbology
        Esri::ArcGISRuntime::Graphic* pointGraphic = new Esri::ArcGISRuntime::Graphic(dumeBeach, pointSymbol, this);
        overlay->graphics()->append(pointGraphic);

        mapView->graphicsOverlays()->append(overlay);
	}
}