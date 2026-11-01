#include "griblite/VerticalInterpolator.hpp"
#include "griblite/Interpolation.hpp"
#include <cmath>
#include <algorithm>

namespace griblite {
    double VerticalInterpolator::pressureFromAltitude(double z) noexcept {
        constexpr double p0 = 1013.25;
        constexpr double T0 = 288.15;
        constexpr double L = 0.0065;
        constexpr double gM_over_RL = 5.255;
        double term = 1.0 - L * z / T0;
        if (term <= 0.0) {
            term = 1e-6;
        }
        return p0 * std::pow(term, gM_over_RL);
    }
    std::optional<float> VerticalInterpolator::sampleScaler(
        const GribDataset& ds,
        const std::string& parameter,
        double lat,
        double lon,
        double altitudeMeters,
        int forecastHour) noexcept {
        double pTarget = pressureFromAltitude(altitudeMeters);
        std::vector<double> levels;
        for (const auto& f : ds.fields()) {
            if (f.parameter == parameter && f.levelType == "100") {
                levels.push_back(f.levelValue);
            }
        }
        if (levels.size() < 2) {
            return std::nullopt;
        }
        std::sort(levels.begin(), levels.end());
        double pLow = 0.0, pHigh = 0.0;
        bool found = false;
        for (size_t i = 0; i + 1 < levels.size(); ++i) {
            double a = levels[i];
            double b = levels[i + 1];
            if ((pTarget <= a && pTarget >= b) || (pTarget >= a && pTarget <= b)) {
                pLow = a;
                pHigh = b;
                found = true;
                break;
            }
        }
        if (!found) {
            return std::nullopt;
        }
        auto fLow = ds.findField(parameter, "100", pLow, forecastHour);
        auto fHigh = ds.findField(parameter, "100", pHigh, forecastHour);
        if (!fLow || !fHigh) {
            return std::nullopt;
        }
        auto vLow = Interpolation::bilinear(**fLow, lat, lon);
        auto vHigh = Interpolation::bilinear(**fHigh, lat, lon);
        if (!vLow || !vHigh) {
            return std::nullopt;
        }
        double logPL = std::log(pLow);
        double logPH = std::log(pHigh);
        double logPT = std::log(pTarget);
        double t = (logPT - logPL) / (logPH - logPL);
        float v = static_cast<float>(*vLow + t * (*vHigh - *vLow));
        return v;
    }   
}