#pragma once
#include "griblite/model/GribDataset.hpp"
#include "griblite/bitstream/BitReader.hpp"
#include "griblite/bitstream/ByteBuffer.hpp"
#include <optional>

namespace griblite {
    class GRIB1Reader {
        public: 
            std::optional<GribDataset> parse(const uint8_t* data, size_t size) noexcept;
        private:
            template <typename T> int64_t parseSignMagnitude(T rawValue, size_t numBits) noexcept;
            float convertIBMToIEEE(uint32_t IBMBits) noexcept;
            bool isBitSet(const std::vector<uint8_t>& bitmap, size_t linearIndex, uint16_t unusedBits) noexcept;
            bool parseSection0(BitReader& br) noexcept;
            bool parsePDS(BitReader& br, GribField& field, bool& hasGDS, bool& hasBMS) noexcept;
            bool parseGDS(BitReader& br, GribField& field, size_t totalLength, size_t startByte)  noexcept;
            bool parseBMS(BitReader& br, std::vector<uint8_t>& bitmap, uint16_t& unusedBits, size_t totalLength, size_t startByte) noexcept;
            bool parseBDS(BitReader& br, GribField& field, const std::vector<uint8_t>& bitmap, uint16_t unusedBits, size_t totalLength, size_t startByte) noexcept;
    };
}