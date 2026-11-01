#pragma once
#include <string>
#include <optional>
#include "griblite/model/GribDataset.hpp"
#include "griblite/bitstream/BitReader.hpp"

namespace griblite {
    class GRIB2Reader {
        public:
            std::optional<GribDataset> parse(const uint8_t* data, size_t size) noexcept;
        private:
            bool parseSection0(BitReader& br, GribDataset& dataset) noexcept;
            bool parseSection1(BitReader& br, GribDataset& dataset) noexcept;
            bool parseSection3(BitReader& br, GribDataset& dataset) noexcept;
            bool parseSection4(BitReader& br, GribDataset& dataset) noexcept;
            bool parseSection5(BitReader& br, GribDataset& dataset) noexcept;
            bool parseSection6(BitReader& br, GribDataset& dataset) noexcept;
            bool parseSection7(BitReader& br, GribDataset& dataset) noexcept;
    };
}