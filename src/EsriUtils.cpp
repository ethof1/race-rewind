#include "EsriUtils.h"

#include <ArcGISRuntimeEnvironment.h>

#include <QString>

namespace rrewind
{
	bool configureEsriApi()
	{
        bool success = true;

        // Get the ArcGIS API key as an environment variable and set it on the runtime
        QString apiKey = qgetenv("ARCGIS_API_KEY");
        if (!apiKey.isEmpty())
        {
            Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);
            success = true;
        }
        else
        {
            qWarning() << "Could not configure ArcGIS API: An environment variable "
                << "named \"ARCGIS_API_KEY\" must be set.";
            success = false;
        }

        return success;
	}
}