#include "griblite/export/CSVExporter.hpp"
#include <fstream>
#include <iomanip>

namespace griblite {
    bool CSVExporter::write(const GribField& field,
        const std::string& path,
        Mode mode) noexcept {
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
        if (mode == Mode::LatLonValue) {
            out << "Lat,Lon,Value\n";
        } else { 
            out << "i,j,Lat,Lon,Value\n";
        }
        const double dLat = (g.lat2 - g.lat1) / double(ny - 1);
        const double dLon = (g.lon2 - g.lon1) / double(nx - 1);
        for (size_t j = 0; j < ny; ++j) {
            double lat = g.lat1 + j * dLat;
            for (size_t i = 0; i < nx; ++i) {
                double lon = g.lon1 + i * dLon;
                float value = field.values[j * nx + i];
                if (mode == Mode::LatLonValue) {
                    out << std::fixed << std::setprecision(6) 
                        << lat << "," << lon << "," << value << "\n";
                } else { 
                    out << i << "," << j << "," 
                    << std::fixed << std::setprecision(6) 
                    << lat << "," << lon << "," << value << "\n";
                }
            }
        }
        return true;
    }
}