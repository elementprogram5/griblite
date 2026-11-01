#pragma once

#include <cstdint>

namespace griblite {
    enum class GridType {
        RegularLatLon,
        Mercator,
        RotatedLatLon,
        LambertConformal,
        PolarStereographic,
        Unknown
    };
    struct GridDefinition {
        // Internal decoder support status
        bool supported = false;
        // (GDS octet 6)
        uint8_t gribGridType = 255;
        GridType type = GridType::Unknown;
        uint32_t nx = 0;
        uint32_t ny = 0;
        double lat1 = 0.0;
        double lon1 = 0.0;
        double lat2 = 0.0;
        double lon2 = 0.0;
        double latin = 0.0; //mercator
        double latin1 = 0.0;
        double latin2 = 0.0;
        double lov = 0.0;      // orientation longitude
        double latSP = 0.0;    // latitude of southern pole
        double lonSP = 0.0;    // longitude of southern pole
        bool projectionSouth = false;
        // Degrees for regular lat/lon grids.
        // May represent other units for other projections later.
        double dx = 0.0;
        double dy = 0.0;
        bool isGlobal = false;
        // Resolution and component flag (GDS octet 17, Table 7)
        bool hasDirectionIncrements = false;
        bool earthIsOblate = false;
        bool vectorComponentsResolved = false;
        // Scan mode flags (GDS octet 28, Table 8)
        uint8_t scanMode = 0;
        bool iDirectionNegative = false;
        bool jDirectionPositive = true;
        bool adjacentPointsInJDirection = false;
        // TODO: Lambert conformal parameters, Polar stereographic parameter, Rotated lat/lon parameters
    };
}