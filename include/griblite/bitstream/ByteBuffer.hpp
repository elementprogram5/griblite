#pragma once
#include <cstddef>
#include <cstdint>

namespace griblite {
    class ByteBuffer {
        public:
            ByteBuffer(const uint8_t* data, size_t size) noexcept;
            size_t size() const noexcept {return m_size;}
            const uint8_t* data() const noexcept {return m_data;}
            uint8_t operator[](size_t i) const noexcept;
            bool read(size_t offset, uint8_t& out) const noexcept;
        private:
            const uint8_t* m_data;
            size_t m_size;
    }; 
}