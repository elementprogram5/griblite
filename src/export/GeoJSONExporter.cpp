#include "griblite/export/GeoJSONExporter.hpp"
#include <fstream>
#include <iomanip>

namespace griblite {
    bool GeoJSONExporter::write(const GribField& field, const std::string& path, Mode mode) noexcept {
        std::ofstream out(path);
        if (!out) {
            return false;
        }
        const auto& g = field.grid;
        const size_t nx = g.nx;
        const size_t ny = g.ny;
        if (nx == 0 || ny == 0 || field.values.size() != nx * ny) {
            return false;
        }
        const double dLat = (g.lat2 - g.lat1) / double(ny - 1);
        const double dLon = (g.lon2 - g.lon1) / double(nx - 1);
        out << "{\n";
        out << " \"type\": \"FeatureCollection\",\n";
        out << " \"features\": [\n";
        bool first = true;
        for (size_t j = 0; j < ny; ++j) {
            double lat = g.lat1 + j * dLat;
            for (size_t i = 0; i < nx; ++i) {
                double lon = g.lon1 + j * dLon;
                float value = field.values[j * nx + i];
                if (!first) {
                    out << ",\n";
                }
                first = false;
                out << "    {\n";
                out << "      \"type\": \"Feature\",\n";
                out << "      \"geometry\": {\n";
                out << "        \"type\": \"Point\",\n";
                out << "        \"coordinates\": [" << std::fixed << std::setprecision(6)
                    << lon << ", " << lat << "]\n";
                out << "      },\n";
                out << "      \"properties\": {\n";
                if (mode == Mode::IJLatLonValue) {
                    out << "        \"i\": " << i << ",\n";
                    out << "        \"j\": " << j << ",\n";
                }
                out << "        \"value\": " << value << "\n";
                out << "      }\n";
                out << "    }";
            }
        }
        out << "\n  ]\n";
        out << "}\n";
        return true;
    }
}