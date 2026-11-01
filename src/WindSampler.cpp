#include "griblite/WindSampler.hpp"
#include "griblite/Interpolation.hpp"
#include "griblite/VerticalInterpolator.hpp"

namespace griblite {
    std::optional<WindVector> WindSampler::getWindAtLevel(
        const GribDataset& ds,
        double lat,
        double lon,
        double levelValue,
        int forecastHour) noexcept {
        auto ugrd = ds.findField("UGRD", "100", levelValue, forecastHour);
        auto vgrd = ds.findField("VGRD", "100", levelValue, forecastHour);
        if (!ugrd || !vgrd) {
            return std::nullopt;
        }
        auto u = Interpolation::bilinear(**ugrd, lat, lon);
        auto v = Interpolation::bilinear(**vgrd, lat, lon);
        if (!u || !v) {
            return std::nullopt;
        }
        return WindVector { *u, *v};
    }
    std::optional<WindVector> WindSampler::sample3D(
        const GribDataset& ds,
        double lat,
        double lon,
        double altitudeMeters,
        int forecastHour) noexcept {
        auto u = VerticalInterpolator::sampleScaler(ds, "UGRD", lat, lon, altitudeMeters, forecastHour);
        auto v = VerticalInterpolator::sampleScaler(ds, "VGRD", lat, lon, altitudeMeters, forecastHour);
        if (!u || !v) {
            return std::nullopt;
        }
        return WindVector { *u, *v};
    }
}