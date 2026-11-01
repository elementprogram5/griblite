#pragma once
#include "model/GribField.hpp"
#include <optional>

namespace griblite {
    class Interpolation {
        public: 
            static std::optional<float> bilinear(
                const GribField& field,
                double lat,
                double lon) noexcept;
    };
}