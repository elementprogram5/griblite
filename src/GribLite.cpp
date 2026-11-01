#include "griblite/GribLite.hpp"
#include "griblite/codec/grib2/GRIB2Reader.hpp"
#include "griblite/codec/grib1/GRIB1Reader.hpp"
#include <fstream>
#include <iostream>

namespace griblite
{
    static const uint8_t GRIB_MAGIC[4] = { 'G', 'R', 'I', 'B' };
    static const uint8_t END_MAGIC[4] = { '7', '7', '7', '7' };
    static const uint8_t* findNextGRIB(const uint8_t* data, size_t size, size_t& offset) {
        for (; offset + 4 <= size; ++offset) {
            if (data[offset] == GRIB_MAGIC[0] &&
                data[offset + 1] == GRIB_MAGIC[1] &&
                data[offset + 2] == GRIB_MAGIC[2] &&
                data[offset + 3] == GRIB_MAGIC[3]) {
                    return data + offset;
            }
        }
        return nullptr;
    }
    std::optional<GribDataset> GribLite::read(const uint8_t* data, size_t size) noexcept {
        GribDataset dataset;
        size_t offset = 0;
        while (true) {
            const uint8_t* msg = findNextGRIB(data, size, offset);
            if (!msg) {
                break;
            }
            std::cout << "[GribLite::read] size = " << size << "\n";
            uint8_t edition = msg[7];
            size_t remaining = size - offset;
            std::optional<GribDataset> single;
            if (edition == 1) {
                std::cout << "[GribLite::read] dispatching to GRIB1Reader\n";
                GRIB1Reader reader;
                single = reader.parse(msg, remaining);
            }
            else if (edition == 2) {
                std::cout << "[GribLite::read] dispatching to GRIB2Reader\n";
                GRIB2Reader reader;
                single = reader.parse(msg, remaining);
            }
            else {
                // Unknown edition
                std::cout << "[GribLite::read] unsupported edition\n";
                break;
            }
            if (!single) {
                break;
            }
            for (const auto& f : single->fields()) {
                dataset.addField(f);
            }
            size_t endPos = offset;
            while (endPos + 4 <= size) {
                if (data[endPos]     == END_MAGIC[0] &&
                    data[endPos + 1] == END_MAGIC[1] &&
                    data[endPos + 2] == END_MAGIC[2] &&
                    data[endPos + 3] == END_MAGIC[3]) {
                    endPos += 4;
                    break;
                }
                ++endPos;
            }
            if (endPos <= offset) {
                break;
            }
            offset = endPos;
        }
        if (dataset.fields().empty()) {
            return std::nullopt;
        }
        return dataset;
    }
    std::optional<GribDataset> GribLite::readFile(const std::string& path) noexcept {
        std::ifstream in (path, std::ios::binary);
        if (!in) {
            return std::nullopt;
        }
        in.seekg(0, std::ios::end);
        std::streamsize sz = in.tellg();
        in.seekg(0, std::ios::beg);
        if (sz <= 0 ) {
            return std::nullopt;
        }
        std::vector<uint8_t> buf(static_cast<size_t>(sz));
        if (!in.read(reinterpret_cast<char*>(buf.data()), sz)) {
            return std::nullopt;
        }
        return read(buf.data(), buf.size());
    }
} 
