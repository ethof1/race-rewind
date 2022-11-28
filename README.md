# Race Rewind

## Overview
This project demonstrates playback of Formula 1 telemetry data using an [Esri File Geodatabase](https://desktop.arcgis.com/en/arcmap/10.3/manage-data/administer-file-gdbs/file-geodatabases.htm). 
It uses the [FastF1 python project](https://theoehrly.github.io/Fast-F1) to retrieve raw telemetry data, and then compiles this data into a File Geodatabase. The telemetry data can then be 
"played back" on a map, using the [Esri ArcGIS Runtime API for Qt](https://developers.arcgis.com/qt/).

Currently, the application will only playback position data for each vehicle. Other telemetry is available (e.g., speed, throttle percentage, etc.), but is not currently used by this application. Finally, the specific race/session is currently hardcoded, but could be changed by passing additional parameters to the Fast F1 script.

## About the Telemetry Data
Each raw incoming telemetry entry contains the following data:
* A "Driver ID" label string, which identifies the driver for which the telemetry entry applies
* An ISO-8601 formatted timestamp string, including milliseconds (telemetry is sampled at approimately 0.5s intervals during the race)
* Car position as a local-space, cartestian (x,y) offset from a fixed, arbitrary location selected within the race track. The units are in decimeters

This telemetry data is first pulled using the Fast F1 `python` library, using a local script that specifies the race and session to pull (see [f1-cache-pull.py](scripts/f1-cache-pull.py)). The script dumps the data to a `csv` file, as an alternative to inter-process communication between `python` and `c++`. Finally, the [TelemetryCacheLoader](src/TelemetryCacheLoader.cpp) reads the `csv` file and converts the telemetry entries into the following structured data:
* Driver ID string (unchanged)
* Millisecond offset from the earliest telemetry entry
* Car position as a `Point` object containing geodetic coordinates (`EPSG:4326`). The local-space coordinate was converted to geodetic using a topocentric coordinate conversion using `proj`. See [LocalSpaceConverter](src/LocalSpaceConverter.cpp)
  * The projection anchor point was manually determined, as it is not otherwise published

The telemetry entries are then stored in the Geodatabase. See [GeodatabaseCompiler](src/GeodatabaseCompiler.cpp).

## Demo Video
View the [demo video](https://1drv.ms/v/s!At-cIsI_dfVhgRZDpAjsDLiCoZy7?e=E94KjF), 
which shows compiling telemetry data from the 2022 Italian Grand Prix into a File Geodatabase, and then using the Geodatabase to play back each 
vehicle's position trace during the race.
