#pragma once
#include "model/GribDataset.hpp"
#include <optional>

namespace griblite {
    class VerticalInterpolator {
        public:
            static double pressureFromAltitude(double altitudeMeters) noexcept;
            static std::optional<float> sampleScaler(const GribDataset& ds,
                const std::string& parameter,
                double lat,
                double lon,
                double altitudeMeters,
                int forecastHours = 0) noexcept;
    };
}