#pragma once
#include "griblite/model/GribField.hpp"
#include <string>

namespace griblite {
    class CSVExporter {
        public:
            enum class Mode {
                LatLonValue,
                IJLatLonValue
            };
            static bool write (const GribField& field, 
                const std::string& path, 
                Mode mode = Mode::LatLonValue) noexcept;
    };
}