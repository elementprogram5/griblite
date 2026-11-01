#include <gtest/gtest.h>
#include "griblite/bitstream/ByteBuffer.hpp"
#include "griblite/bitstream/BitReader.hpp"

namespace griblite {
namespace bitstream {

    TEST(BitReader, ReadsBitsCorrectly) {
        uint8_t data[] = { 0b10110110, 0b01011100 };
        ByteBuffer buf(data, 2);
        BitReader br(buf);

        uint64_t v;

        br.readBits(3, v);  // 101
        EXPECT_EQ(v, 0b101);

        br.readBits(5, v);  // 10110
        EXPECT_EQ(v, 0b10110);

        br.readBits(4, v);  // 0101
        EXPECT_EQ(v, 0b0101);
    }
}
}