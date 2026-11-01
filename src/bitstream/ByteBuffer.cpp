#include "griblite/bitstream/ByteBuffer.hpp"

namespace griblite {
    ByteBuffer::ByteBuffer(const uint8_t* data, size_t size) noexcept
        : m_data(data), m_size(size) {}
    uint8_t ByteBuffer::operator[](size_t i) const noexcept {
        return (i < m_size) ? m_data[i] : 0;
    }
    bool ByteBuffer::read(size_t offset, uint8_t& out) const noexcept {
        if (offset >= m_size) {
            return false;
        }
        out = m_data[offset];
        return true;
    }
} 