# Race Rewind

## Overview
This project demonstrates playback of Formula 1 telemetry data using an [Esri File Geodatabase](https://desktop.arcgis.com/en/arcmap/10.3/manage-data/administer-file-gdbs/file-geodatabases.htm). 
It uses the [FastF1 python project](https://theoehrly.github.io/Fast-F1) to retrieve raw telemetry data, and then compiles this data into a File Geodatabase. The telemetry data can then be 
"played back" on a map, using the [Esri ArcGIS Runtime API for Qt](https://developers.arcgis.com/qt/).

Currently, the application will only playback position data for each vehicle.

## Demo Video
View the [demo video](https://1drv.ms/v/s!At-cIsI_dfVhgRZDpAjsDLiCoZy7?e=E94KjF), 
which shows compiling telemetry data from the 2022 Italian Grand Prix into a File Geodatabase, and then using the Geodatabase to play back each 
vehicle's position trace during the race.
