#include <gtest/gtest.h>
#include "griblite/GribLite.hpp"

namespace griblite {

TEST(GRIB1, LoadsRegularLatLonFile) {
    auto ds = GribLite::readFile("data/grib1/ll_10_20.grib");
    ASSERT_TRUE(ds.has_value());

    const auto& fields = ds->fields();
    ASSERT_GT(fields.size(), 0);

    const auto& f = fields[0];
    EXPECT_GT(f.grid.nx, 0);
    EXPECT_GT(f.grid.ny, 0);
    EXPECT_FALSE(f.parameter.empty());
}

TEST(GRIB1, LoadsMercatorFile) {
    auto ds = GribLite::readFile("data/grib1/mgradient.grb");
    ASSERT_TRUE(ds.has_value());

    const auto& fields = ds->fields();
    ASSERT_GT(fields.size(), 0);

    const auto& f = fields[0];
    EXPECT_GT(f.grid.nx, 0);
    EXPECT_GT(f.grid.ny, 0);
    EXPECT_FALSE(f.parameter.empty());
}

TEST(GRIB1, LoadsLambertConformalFile) {
    auto ds = GribLite::readFile("data/grib1/lcgradient.grb");
    ASSERT_TRUE(ds.has_value());

    const auto& fields = ds->fields();
    ASSERT_GT(fields.size(), 0);

    const auto& f = fields[0];
    EXPECT_GT(f.grid.nx, 0);
    EXPECT_GT(f.grid.ny, 0);
    EXPECT_FALSE(f.parameter.empty());
}

TEST(GRIB1, LoadsPolarStereographicFile) {
    auto ds = GribLite::readFile("data/grib1/psgradient.grb");
    ASSERT_TRUE(ds.has_value());

    const auto& fields = ds->fields();
    ASSERT_GT(fields.size(), 0);

    const auto& f = fields[0];
    EXPECT_GT(f.grid.nx, 0);
    EXPECT_GT(f.grid.ny, 0);
    EXPECT_FALSE(f.parameter.empty());
}

TEST(GRIB1, ValuesAreFinite) {
    auto ds = GribLite::readFile("data/grib1/ll_10_20.grib");
    ASSERT_TRUE(ds.has_value());

    for (const auto& f : ds->fields()) {
        for (float v : f.values) {
            EXPECT_TRUE(std::isfinite(v));
        }
    }
}
 
}