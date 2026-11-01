#include <gtest/gtest.h>
#include "griblite/GribLite.hpp"

namespace griblite {

TEST(GRIB2, LoadsRealFile) {
    auto ds = GribLite::readFile("data/grib2/ref_simple_packing.grib2");
    ASSERT_TRUE(ds.has_value());

    const auto& fields = ds->fields();
    ASSERT_GT(fields.size(), 0);

    const auto& f = fields[0];
    EXPECT_GT(f.grid.nx, 0);
    EXPECT_GT(f.grid.ny, 0);
    EXPECT_FALSE(f.parameter.empty());
}

TEST(GRIB2, ValuesAreFinite) {
    auto ds = GribLite::readFile("data/grib2/ref_simple_packing.grib2");
    ASSERT_TRUE(ds.has_value());

    for (const auto& f : ds->fields()) {
        for (float v : f.values) {
            EXPECT_TRUE(std::isfinite(v));
        }
    }
}
}