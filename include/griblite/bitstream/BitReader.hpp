#pragma once
#include <cstdint>
#include <cstddef>
#include "ByteBuffer.hpp"

namespace griblite {
    class BitReader {
        public:
            explicit BitReader(const ByteBuffer& buffer) noexcept;
            bool readBits(uint32_t nbits, uint64_t& out) noexcept;
            bool readSignedBits(uint32_t nbits, int64_t& out) noexcept;
            bool skipBits(uint32_t nbits) noexcept;
            bool alignToByte() noexcept;
            bool eof() const noexcept;
            size_t getBitPosition() const noexcept { return m_bitPos; }
            void setBitPosition(size_t position) noexcept {m_bitPos = position;}
            size_t getBitsRemaining() const noexcept;
        private:
            const ByteBuffer& m_buf;
            size_t m_bitPos;
    };
}