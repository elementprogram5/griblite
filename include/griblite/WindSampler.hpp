#pragma once
#include "model/GribDataset.hpp"
#include <optional>

namespace griblite {
    struct WindVector {
        float u;
        float v;
    };
    class WindSampler {
        public:
            static std::optional<WindVector> getWindAtLevel(
                const GribDataset& ds,
                double lat,
                double lon,
                double levelValue,
                int forecastHour = 0) noexcept;
            static std::optional<WindVector> sample3D(
                const GribDataset& ds,
                double lat,
                double lon,
                double altitudeMeters,
                int forecastHour = 0) noexcept;
    };
}