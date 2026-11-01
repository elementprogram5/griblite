#include "griblite/bitstream/BitWriter.hpp"

namespace griblite {
    BitWriter::BitWriter() noexcept
        : m_buf(1,0), m_bitPos(0) {}
    void BitWriter::writeBits(uint64_t value, uint32_t nbits) noexcept {
        for (int i = nbits - 1; i >= 0; --i) {
            uint8_t bit = (value >> i) & 1;
            if (m_bitPos == 8) {
                m_buf.push_back(0);
                m_bitPos = 0;
            }
            uint8_t mask = (1 << (7 - m_bitPos));
            if (bit) m_buf.back() |= mask;
            m_bitPos++;
        }
    }
    void BitWriter::writeSignedBits(uint64_t value, uint32_t nbits) noexcept {
        writeBits(static_cast<uint64_t>(value) & ((1ULL << nbits)-1), nbits);
    }
    void BitWriter::alignToByte() noexcept {
        if (m_bitPos == 0) {
            return;
        }
        m_bitPos = 0;
        m_buf.push_back(0);
    }
    std::vector<uint8_t> BitWriter::takeBuffer() noexcept {
        auto out = std::move(m_buf);
        m_buf = {0};
        m_bitPos = 0;
        return out;
    }
}