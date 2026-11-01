cmake_minimum_required(VERSION 3.20)
project(GribLite LANGUAGES CXX)

# -----------------------------
# Global settings
# -----------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# -----------------------------
# Library target
# -----------------------------
add_library(griblite
    # Core
    src/GribLite.cpp
    src/model/GribDataset.cpp
    # Bitstream
    src/bitstream/ByteBuffer.cpp
    src/bitstream/BitReader.cpp
    src/bitstream/BitWriter.cpp
    # GRIB2
    src/codec/grib2/GRIB2Reader.cpp
    src/codec/grib2/GRIB2ParameterTable.cpp
    # GRIB1
    src/codec/grib1/GRIB1Reader.cpp
    src/codec/grib1/GRIB1ParameterTable.cpp
    # Exporters
    src/export/CSVExporter.cpp
    src/export/GeoJSONExporter.cpp
)

target_include_directories(griblite
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -----------------------------
# Tests 
# -----------------------------
enable_testing()
add_subdirectory(tests)

add_executable(decode_test
    examples/decode_test.cpp
)

target_link_libraries(decode_test
    PRIVATE griblite
)