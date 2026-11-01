#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <cmath>
#include "GridDefinition.hpp"

namespace griblite {
    struct GribField {
        int edition = 0;
        std::string center;
        std::string parameter; //UGRD, VGRD
        std::string discipline;
        std::string levelType;
        double levelValue = 0.0;
        std::string refTime;
        int forecastHours = 0;
        GridDefinition grid;
        double referenceValue = 0.0;
        int16_t binaryScale = 0;
        int16_t decimalScale = 0;
        uint16_t bitsPerValue = 0;
        uint16_t dataTemplate = 0;
        std::vector<float> values;
        std::vector<uint8_t> bitmap;
        bool hasBitmap() const noexcept { return !bitmap.empty(); }
        bool isValid() const noexcept { 
            if (grid.nx <= 0 || grid.ny <= 0 || values.empty()) {
                return false;
            }
            return static_cast<size_t>(grid.nx) * static_cast<size_t>(grid.ny) == values.size();
        }
        bool isMissing(size_t index) const noexcept {
            return index >= values.size() || !std::isfinite(values[index]);
        }
    };
}