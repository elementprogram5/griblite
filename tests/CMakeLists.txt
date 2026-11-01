# Tests CMakeLists.txt

# -----------------------------
# Test executable
# -----------------------------
add_executable(griblite_tests
    test_bitreader.cpp
    test_bitreader_negative.cpp
    test_grib1_structural.cpp
    test_grib2_structural.cpp
    test_negative_corruption.cpp
)

# -----------------------------
# Link against GRIBLite + GTest
# -----------------------------
target_link_libraries(griblite_tests
    PRIVATE
        griblite
        GTest::gtest_main
)

# -----------------------------
# Register test suite
# -----------------------------
include(GoogleTest)
gtest_discover_tests(griblite_tests)
