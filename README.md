# GRIBLITE

**GRIBLITE** is an independently developed C++20 library for decoding GRIB (GRIdded Binary) meteorological data files, with a particular focus on native Windows compatibility.

The project was created to provide a way to work with GRIB data directly from C++ applications on Windows, without depending on established GRIB decoding libraries like ECCODES that may require additional environments or compatibility workarounds.

## Features

### GRIB1

- **Binary parsing:** Custom byte-level and bit-level reading utilities.
- **Message sections:** Parses the sections that make up GRIB1 messages.
- **Data packing:** Supports GRIB1 packed data decoding.
- **Bitmap handling:** Supports bitmaps used to represent missing data.
- **Grid representations:** Supports the following grid types:
  - Regular latitude-longitude
  - Rotated latitude-longitude
  - Mercator
  - Polar stereographic
  - Lambert conformal

### GRIB2

GRIB2 development is currently in its early stages. The codebase contains foundational structures and implementation groundwork, but complete GRIB2 decoding is not yet supported.

### Testing and Examples

- **GoogleTest:** Unit tests for individual components and decoding functionality.
- **Usage examples:** Example programs demonstrate how to integrate and use the GRIBLITE library in C++ projects.

## Project Status

GRIBLITE is actively under development.

GRIB1 is the primary focus, with implemented support for packing, bitmaps, and several common grid representations. Support for additional grid definitions and other GRIB features is an ongoing goal.

GRIB2 currently consists primarily of foundational work.

The long-term goal is to build a lightweight, reliable, and cross-platform GRIB decoding library.

## Motivation

Working with GRIB data in native Windows development environments can be challenging because established GRIB decoding libraries may require additional dependencies, compatibility layers, or alternative environments.

GRIBLITE was created to explore an alternative: a decoder library written in modern C++ that can be built and integrated directly into Windows applications without requiring WSL or another Linux environment.

The project also serves as an opportunity to understand and implement the underlying binary format rather than relying on an existing decoding library.

## Project Structure

```text
GRIBLITE/
├── CMakeLists.txt
├── examples/
├── include/
│   └── griblite/
│       ├── bitstream/
│       └── grib1/
├── src/
│   ├── bitstream/
│   ├── grib1/
│   └── grib2/
└── tests/
```

- `include/` — Public library headers.
- `src/` — Library implementation.
- `examples/` — Example programs demonstrating library usage.
- `tests/` — GoogleTest unit and component tests.

## Using the Library

GRIBLITE is currently a decoder library rather than a standalone command-line application.

To see how to integrate and use the library, refer to the source files in the [`examples/`](examples/) directory. These examples demonstrate how to work with the library's interfaces and decode GRIB data from a C++ program.

## Requirements

- C++20 compatible compiler
- CMake 3.20 or newer
- Git
- Internet connection during initial configuration if GoogleTest needs to be downloaded through CMake FetchContent

## Building

Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/GRIBLITE.git
cd GRIBLITE
```

Configure the project:

```bash
cmake -S . -B build
```

Build the library and its configured targets:

```bash
cmake --build build --config Release
```

These commands can be run from a native Windows development environment with a compatible C++ compiler and CMake installation.

## Running Tests

GRIBLITE uses GoogleTest for unit and component testing.

Run the tests using CTest:

```bash
ctest --test-dir build --output-on-failure -C Release
```

## Design Goals

1. **Native Windows support:** Provide a GRIB decoding library that can be built and integrated into native Windows C++ projects without requiring WSL or another Linux environment.
2. **Incremental format support:** Expand GRIB1 and GRIB2 support through clearly defined development milestones.
3. **Portability:** Maintain compatibility with modern C++ compilers on Windows and Linux.

## Background

GRIB is a binary format used to represent and exchange gridded meteorological data. Decoding it requires handling structured message sections, bit-packed values, missing data representations, and different geographic grid projections.
GRIBLITE is my attempt to implement that decoding process from the ground up, building a deeper understanding of binary data processing, C++, and meteorological data formats.

## Technologies

- **C++20** — Core implementation
- **CMake** — Build system
- **GoogleTest** — Unit testing

## Author

**ElementProgram5**

Developed as a personal software engineering project.
