#include "griblite/bitstream/BitReader.hpp"

namespace griblite {
    BitReader::BitReader(const ByteBuffer& buffer) noexcept 
        : m_buf(buffer), m_bitPos(0) {}
    size_t BitReader::getBitsRemaining() const noexcept {
        size_t maxBits = m_buf.size() * 8;
        if (m_bitPos >= maxBits) return 0;
        return maxBits - m_bitPos;
    }
    bool BitReader::eof() const noexcept {
        return(m_bitPos /  8) >= m_buf.size();
    }
    bool BitReader::skipBits(uint32_t nbits) noexcept {
        size_t maxBits = m_buf.size() * 8;
        if (m_bitPos > maxBits || nbits > (maxBits - m_bitPos)) {
            m_bitPos = maxBits; 
            return false;
        }
        m_bitPos += nbits;
        return true;
    }
    bool BitReader::alignToByte() noexcept {
        size_t mod = m_bitPos % 8;
        if (mod == 0) {
            return !eof();
        }
        return skipBits(8 - mod);
    }
    bool BitReader::readBits(uint32_t nbits, uint64_t& out) noexcept {
        if (nbits == 0) {
            out = 0;
            return true;
        }
        if (nbits > 64 || nbits > getBitsRemaining()) {
            return false;
        }
        out = 0;
        uint32_t bitsLeftToRead = nbits;
        while (bitsLeftToRead > 0) {
            size_t byteIndex = m_bitPos >> 3;
            uint32_t bitOffset = m_bitPos & 7;
            uint32_t bitsAvailableInByte = 8 - bitOffset;
            uint32_t chunkBits = (bitsLeftToRead < bitsAvailableInByte) ? bitsLeftToRead : bitsAvailableInByte;    
            uint8_t byteData = m_buf[byteIndex];
            uint8_t shiftRightAmount = bitsAvailableInByte - chunkBits;
            uint8_t mask = (1 << chunkBits) - 1;
            uint64_t extractedValue = (byteData >> shiftRightAmount) & mask;
            out = (out << chunkBits) | extractedValue;
            m_bitPos += chunkBits;
            bitsLeftToRead -= chunkBits;
        }
        return true;
    }
    bool BitReader::readSignedBits(uint32_t nbits, int64_t& out) noexcept {
        uint64_t raw = 0;
        if (!readBits(nbits, raw)) {
            return false;
        }
        uint64_t signMask = 1ULL << (nbits - 1);
        if (raw & signMask) {
            uint64_t extendMask = (~0ULL) << nbits;
            raw |= extendMask;
        }
        out = static_cast<int64_t>(raw);
        return true;
    }
}