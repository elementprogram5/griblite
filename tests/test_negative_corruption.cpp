#include <gtest/gtest.h>
#include <fstream>
#include <iterator>
#include <vector>
#include "griblite/GribLite.hpp"

namespace griblite {
static std::vector<uint8_t> load(const char* path) {
    std::ifstream in(path, std::ios::binary);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(in)),
                                 std::istreambuf_iterator<char>());
}

TEST(Negative, TruncateRealGRIB) {
    auto data = load("data/grib1/regular_ll.grib");
    ASSERT_GT(data.size(), 100);

    data.resize(50); // truncate badly

    auto ds = GribLite::read(data.data(), data.size());
    EXPECT_FALSE(ds.has_value());
}

TEST(Negative, CorruptRandomBytes) {
    auto data = load("data/grib1/ref_grid_172.landmask.grib1");
    ASSERT_GT(data.size(), 100);

    for (int i = 0; i < 20; i++) {
        data[rand() % data.size()] = rand() % 256;
    }

    auto ds = GribLite::read(data.data(), data.size());
    EXPECT_FALSE(ds.has_value());
}
}