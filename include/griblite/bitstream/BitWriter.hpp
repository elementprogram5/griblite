#pragma once
#include <vector>
#include <cstdint>

namespace griblite {
    class BitWriter {
        public:
            BitWriter() noexcept;
            void writeBits(uint64_t value, uint32_t nbits) noexcept;
            void writeSignedBits(uint64_t value, uint32_t nbits ) noexcept;
            void alignToByte() noexcept;
            const std::vector<uint8_t>& buffer() const noexcept { return m_buf; }
            std::vector<uint8_t> takeBuffer() noexcept;
        private:
            std::vector<uint8_t> m_buf;
            uint32_t m_bitPos;
    };
}