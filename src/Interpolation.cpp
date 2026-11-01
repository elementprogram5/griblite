#include "griblite/Interpolation.hpp"
#include <cmath>

namespace griblite {
    std::optional<float> Interpolation::bilinear(
        const GribField& field,
        double lat,
        double lon) noexcept{
        const auto& g = field.grid;
        if (g.nx == 0 || g.ny == 0) {
            return std::nullopt;
        }
        double dx = g.dx;
        double dy = g.dy;
        while (lon < g.lon1) {
            lon += 360.0;
        }
        while (lon > g.lon2) {
            lon -= 360.0;
        }
        double fx = (lon - g.lon1) / dx;
        double fy = (lat - g.lat1) / dy;
        int ix = static_cast<int>(std::floor(fx));
        int iy = static_cast<int>(std::floor(fy));
        if (ix < 0 || ix + 1 >= (int)g.nx) {
            return std::nullopt;
        } 
        if (iy < 0 || iy + 1 >= (int)g.ny) {
            return std::nullopt;
        } 
        double tx = fx - ix;
        double ty = fy - iy;
        auto idx = [&](int x, int y) {
            return y * g.nx + x;
        };
        float v00 = field.values[idx(ix, iy)];
        float v10 = field.values[idx(ix + 1, iy)];
        float v01 = field.values[idx(ix, iy + 1)];
        float v11 = field.values[idx(ix + 1, iy +1)];
        float v0 = v00 * (1 - tx) + v10 * tx;
        float v1 = v01 * (1 - tx) + v11 * tx;
        return v0 * (1 - ty) + v1 * ty;
    }
}