#include <gtest/gtest.h>
#include "griblite/bitstream/ByteBuffer.hpp"
#include "griblite/bitstream/BitReader.hpp"

namespace griblite {
namespace bitstream {


TEST(BitReader, HandlesOutOfBoundsGracefully) {
    uint8_t data[] = { 0xFF };
    ByteBuffer buf(data, 1);
    BitReader br(buf);

    uint64_t v;

    // Reading more bits than exist should fail safely
    EXPECT_FALSE(br.readBits(16, v));
}

TEST(BitReader, HandlesZeroLengthBuffer) {
    ByteBuffer buf(nullptr, 0);
    BitReader br(buf);

    uint64_t v;
    EXPECT_FALSE(br.readBits(1, v));
}

TEST(BitReader, SkipBeyondEndIsSafe) {
    uint8_t data[] = { 0xAA };
    ByteBuffer buf(data, 1);
    BitReader br(buf);

    EXPECT_FALSE(br.skipBits(1000));
}
 
}
}