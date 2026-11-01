#pragma once
#include <string>
#include <optional>
#include "model/GribDataset.hpp"

namespace griblite {
    class GribLite {
        public:
            static std::optional<GribDataset> read(const uint8_t* data, size_t size) noexcept;
            static std::optional<GribDataset> readFile(const std::string& path) noexcept;
    };
}