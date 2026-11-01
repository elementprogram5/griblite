#include <cmath>
#include <cstring>
#include "griblite/codec/grib2/GRIB2Reader.hpp"
#include "griblite/bitstream/ByteBuffer.hpp"
#include "griblite/codec/grib2/GRIB2ParameterTable.hpp"

namespace griblite {
    std::optional<GribDataset> GRIB2Reader::parse(const uint8_t* data, size_t size) noexcept {
        ByteBuffer buf(data, size);
        BitReader br(buf);
        GribDataset dataset;
        if (!parseSection0(br, dataset)) {
            return std::nullopt;
        }
        if (!parseSection1(br, dataset)) {
            return std::nullopt;
        }
        if (!parseSection3(br, dataset)) {
            return std::nullopt;
        }
        if (!parseSection4(br, dataset)) {
            return std::nullopt;
        }
        if (!parseSection5(br, dataset)) {
            return std::nullopt;
        }
        if (!parseSection6(br, dataset)) {
            return std::nullopt;
        }
        if (!parseSection7(br, dataset)) {
            return std::nullopt;
        }
        return dataset;
    }
    bool GRIB2Reader::parseSection0(BitReader& br, GribDataset& dataset) noexcept {
        uint64_t magic = 0;
        for (int i = 0; i< 4; ++i) {
            uint64_t c = 0;
            if (!br.readBits(8, c)) {
                return false;
            }
            magic = (magic << 8) | c;
        }
        if (magic != 0x47495242ULL) {
            return false;
        }
        br.skipBits(16);
        uint64_t discipline = 0;
        if (!br.readBits(8, discipline)) {
            return false;
        }
        uint64_t edition = 0;
        if (!br.readBits(8, edition)) {
            return false;
        }
        if (edition != 2) {
            return false;
        }
        uint64_t totalLength = 0;
        if (!br.readBits(64, totalLength)) {
            return false;
        }
        GribField field;
        field.edition = 2;
        field.discipline = std::to_string(discipline);
        dataset.addField(std::move(field));
        return true;
    }
    bool GRIB2Reader::parseSection1(BitReader& br, GribDataset& dataset) noexcept {
        uint64_t secLen = 0;
        if (!br.readBits(32, secLen)) {
            return false;
        }
        uint64_t secNum = 0;
        if (!br.readBits(8, secNum)) {
            return false;
        }
        if (secNum != 1) {
            return false;
        }
        uint64_t center = 0;
        if (!br.readBits(16, center)) {
            return false;
        }
        uint64_t subcenter = 0;
        if (!br.readBits(16, subcenter)) {
            return false;
        }
        uint64_t masterTable = 0;
        if (!br.readBits(8, masterTable)) {
            return false;
        }
        uint64_t localTable = 0;
        if (!br.readBits(8, localTable)) {
            return false;
        }
        uint64_t timeSignificance = 0;
        if (!br.readBits(8, timeSignificance)) {
            return false;
        }
        uint64_t year = 0;
        if (!br.readBits(16, year)) {
            return false;
        }
        uint64_t month = 0;
        if (!br.readBits(8, month)) {
            return false;
        }
        uint64_t day = 0;
        if (!br.readBits(8, day)) {
            return false;
        }
        uint64_t hour = 0;
        if (!br.readBits(8, hour)) {
            return false;
        }
        uint64_t minute = 0;
        if (!br.readBits(8, minute)) {
            return false;
        }
        uint64_t second = 0;
        if (!br.readBits(8, second)) {
            return false;
        }
        size_t bitsConsumed = 152;
        size_t bitsTotal = static_cast<size_t>(secLen) * 8;
        if (bitsTotal < bitsConsumed) {
            return false;
        }
        size_t remaining = bitsTotal - bitsConsumed;
        if (!br.skipBits(static_cast<uint32_t>(remaining))) {
            return false;
        }
        auto& fields = const_cast<std::vector<GribField>&>(dataset.fields());
        if (fields.empty()) {
            return false;
        }
        GribField& field = fields.front();
        field.center = std::to_string(center);
        field.refTime = std::to_string(year) + "-" +
            (month < 10 ? "0" : "") + std::to_string(month) + "-" +
            (day   < 10 ? "0" : "") + std::to_string(day) + "T" +
            (hour  < 10 ? "0" : "") + std::to_string(hour) + ":" +
            (minute< 10 ? "0" : "") + std::to_string(minute) + ":" +
            (second< 10 ? "0" : "") + std::to_string(second) + "Z";
        return true;
    }
    bool GRIB2Reader::parseSection3(BitReader& br, GribDataset& dataset) noexcept {
        uint64_t secLen = 0;
        if (!br.readBits(32, secLen)) {
            return false;
        }
        uint64_t secNum = 0;
        if (!br.readBits(8, secNum)) {
            return false;
        }
        if (secNum != 3) {
            return false;
        }
        uint64_t gridSource = 0;
        if (!br.readBits(16, gridSource)) {
            return false;
        }
        uint64_t numPoints = 0; 
        if (!br.readBits(32, numPoints)) {
            return false;
        }
        uint64_t optListLen = 0;
        if (!br.readBits(8, optListLen)) {
            return false;
        }
        uint64_t optListInterp = 0;
        if (!br.readBits(8, optListInterp)) {
            return false;
        }
        uint64_t templateNum = 0;
        if (!br.readBits(16, templateNum)) {
            return false;
        }
        auto& fields = const_cast<std::vector<GribField>&>(dataset.fields());
        if (fields.empty()) {
            return false;
        }
        GribField& field = fields.front();
        if (templateNum == 0) {
            GridDefinition grid;
            grid.type = GridType::RegularLatLon;
            //skip stuff fr now 224 bits
            br.skipBits(224);
            uint64_t Ni = 0, Nj = 0;
            br.readBits(32, Ni);
            br.readBits(32, Nj);
            grid.nx = static_cast<uint32_t>(Ni);
            grid.ny = static_cast<uint32_t>(Nj);
            int64_t lat1 = 0, lon1 = 0;
            int64_t lat2 = 0, lon2 = 0;
            br.readSignedBits(32, lat1);
            br.readSignedBits(32, lon1);
            br.skipBits(8);
            br.readSignedBits(32, lat2);
            br.readSignedBits(32, lon2);
            uint64_t di = 0, dj = 0;
            br.readBits(32, di);
            br.readBits(32, dj);
            br.skipBits(8);
            grid.lat1 = lat1 / 1e6;
            grid.lon1 = lon1 / 1e6;
            grid.lat2 = lat2 / 1e6;
            grid.lon2 = lon2 / 1e6;
            grid.dx = di / 1e6;
            grid.dy = dj / 1e6;
            grid.isGlobal = (grid.lon2 - grid.lon1 >= 359.0);
            field.grid = grid;
            size_t bitsUsed = 224 + 32 + 32 + 32 + 32 + 8 + 32 + 32 + 32 + 32 + 8;
            size_t bitsTotal = static_cast<size_t>(secLen) * 8;
            if (bitsTotal > bitsUsed) {
                br.skipBits(static_cast<uint32_t>(bitsTotal - bitsUsed));
            }
            return true;
        }
        size_t bitsTotal = static_cast<size_t>(secLen) * 8;
        br.skipBits(static_cast<uint32_t>(bitsTotal -40));
        return true;
    }
    bool GRIB2Reader::parseSection4(BitReader& br, GribDataset& dataset) noexcept {
        uint64_t secLen = 0;
        if (!br.readBits(32, secLen)) {
            return false;
        }
        uint64_t secNum = 0;
        if (!br.readBits(8, secNum)) {
            return false;
        }
        if (secNum != 4) {
            return false;
        }
        uint64_t coordCount = 0;
        if (!br.readBits(16, coordCount)) {
            return false;
        }
        uint64_t templateNum = 0;
        if (!br.readBits(16, templateNum)) {
            return false;
        }
        auto& fields = const_cast<std::vector<GribField>&>(dataset.fields());
        if (fields.empty()) {
            return false;
        }
        GribField& field = fields.front();
        if (templateNum == 0) {
            uint64_t paramCategory = 0;
            uint64_t paramNumber = 0;
            br.readBits(8, paramCategory);
            br.readBits(8, paramNumber);
            br.skipBits(24);
            uint64_t forecastHours = 0;
            br.readBits(16, forecastHours);
            br.skipBits(8);
            br.skipBits(8); // Time Unit Indicator might need later
            uint64_t forecastTimeUnits = 0;
            br.readBits(32, forecastTimeUnits);
            uint64_t levelType1 = 0;
            br.readBits(8, levelType1);
            int64_t levelScale = 0;
            int64_t levelValue = 0;
            br.readSignedBits(8, levelScale);
            br.readSignedBits(32, levelValue);
            double level = levelValue;
            if (levelScale > 0) {
                level /= std::pow(10.0, levelScale);
            }
            field.forecastHours = static_cast<int>(forecastHours);
            int disciplineInt = 0;
            try {
                disciplineInt = std::stoi(field.discipline);
            } catch(...) {
                disciplineInt = 0;
            }
            auto info = GRIB2ParameterTable::lookup(disciplineInt, 
                static_cast<int>(paramCategory), 
                static_cast<int>(paramNumber));
            if (info) {
                field.parameter = info->shortName;
            } else {
                field.parameter = field.discipline + "." +
                    std::to_string(paramCategory) + "." +
                    std::to_string(paramNumber);
            }
            field.levelType = std::to_string(levelType1);
            field.levelValue = level;
            size_t bitsUsed = 8 + 8 + 24 + 16 + 8 + 8 + 32 + 8 + 8 + 32;
            size_t bitsTotal = static_cast<size_t>(secLen) * 8;
            if (bitsTotal > bitsUsed) {
                br.skipBits(static_cast<uint32_t>(bitsTotal - bitsUsed));
            }
            return true;
        }
        size_t bitsTotal = static_cast<size_t>(secLen) * 8;
        br.skipBits(static_cast<uint32_t>(bitsTotal - 40)); 
        return true;
    }
    bool GRIB2Reader::parseSection5(BitReader& br, GribDataset& dataset) noexcept {
        uint64_t secLen = 0;
        if (!br.readBits(32, secLen)) {
            return false;
        }
        uint64_t secNum = 0;
        if (!br.readBits(8, secNum)) {
            return false;
        }
        if (secNum != 5) {
            return false;
        }
        uint64_t numPoints = 0;
        if (!br.readBits(32, numPoints)) {
            return false;
        }
        uint64_t templateNum = 0;
        if (!br.readBits(16, templateNum)) {
            return false;
        }
        auto& fields = const_cast<std::vector<GribField>&>(dataset.fields());
        if (fields.empty()) {
            return false;
        }
        GribField& field = fields.front();
        field.dataTemplate = static_cast<uint16_t>(templateNum);
        if (templateNum == 0) {
            uint64_t refRaw = 0;
            br.readBits(32, refRaw);
            float refVal;
            std::memcpy(&refVal, &refRaw, sizeof(float));
            field.referenceValue = refVal;
            int64_t binScale = 0;
            br.readSignedBits(16, binScale);
            field.binaryScale = static_cast<int16_t>(binScale);
            int64_t decScale = 0;
            br.readSignedBits(16, decScale);
            field.decimalScale = static_cast<int16_t>(decScale);
            uint64_t bpv = 0;
            br.readBits(16, bpv);
            field.bitsPerValue = static_cast<uint16_t>(bpv);
            br.skipBits(8);
            size_t bitsUsed = 32 + 8 + 32 + 16 + 16 + 16 + 8;
            size_t bitsTotal = static_cast<size_t>(secLen) * 8;
            if (bitsTotal > bitsUsed) {
                br.skipBits(static_cast<uint32_t>(bitsTotal - bitsUsed));
            }
            return true;
        }
        size_t bitsTotal = static_cast<size_t>(secLen) * 8;
        br.skipBits(static_cast<uint32_t>(bitsTotal - 40)); 
        return true;
    }
    bool GRIB2Reader::parseSection6(BitReader& br, GribDataset& dataset) noexcept {
        uint64_t secLen = 0;
        if (!br.readBits(32, secLen)) {
            return false;
        }
        uint64_t secNum = 0;
        if (!br.readBits(8, secNum)) {
            return false;
        }
        if (secNum != 6) {
            return false;
        }
        uint64_t bitmapIndicator = 0;
        if (!br.readBits(8, bitmapIndicator)) {
            return false;
        }
        auto& fields = const_cast<std::vector<GribField>&>(dataset.fields());
        if (fields.empty()) {
            return false;
        }
        GribField& field = fields.front();
        if (bitmapIndicator == 255) {
            field.bitmap.clear();
            size_t bitsUsed = 32 + 8 + 8;
            size_t bitsTotal = static_cast<size_t>(secLen) * 8;
            if (bitsTotal > bitsUsed) {
                br.skipBits(static_cast<uint32_t>(bitsTotal - bitsUsed));
            }
            return true;
        }
        if (bitmapIndicator == 0) {
            size_t numPoints = field.grid.nx * field.grid.ny;
            size_t numBits = numPoints;
            size_t numBytes = (numBits + 7) / 8;
            field.bitmap.resize(numBytes);
            for (size_t i = 0; i < numBytes; ++i) {
                uint64_t byte  = 0;
                if (!br.readBits(8, byte)) {
                    return false;
                }
                field.bitmap[i] = static_cast<uint8_t>(byte);
            }
            size_t bitsUsed = 32 + 8 + 8 + numBytes * 8;
            size_t bitsTotal = static_cast<size_t>(secLen) * 8;
            if (bitsTotal > bitsUsed) {
                br.skipBits(static_cast<uint32_t>(bitsTotal - bitsUsed));
            }
            return true;
        }
        size_t bitsTotal = static_cast<size_t>(secLen) * 8;
        br.skipBits(static_cast<uint32_t>(bitsTotal - 40));
        return true;
    }
    bool GRIB2Reader::parseSection7(BitReader& br, GribDataset& dataset) noexcept {
        uint64_t secLen = 0;
        if (!br.readBits(32, secLen)) {
            return false;
        }
        uint64_t secNum = 0;
        if (!br.readBits(8, secNum)) {
            return false;
        }
        if (secNum != 7) {
            return false;
        }
        auto& fields = const_cast<std::vector<GribField>&>(dataset.fields());
        if (fields.empty()) {
            return false;
        }
        GribField& field = fields.front();
        size_t numPoints = field.grid.nx * field.grid.ny;
        field.values.resize(numPoints);
        bool hasBitmap = !field.bitmap.empty();
        if (field.dataTemplate != 0) {
            size_t bitsTotal = static_cast<size_t>(secLen) * 8;
            br.skipBits(static_cast<uint32_t>(bitsTotal - 40));
            return true;
        }
        double refVal = field.referenceValue;
        double bscale = std::pow(2.0, field.binaryScale);
        double dscale = std::pow(10.0, -field.decimalScale);
        uint16_t bpv = field.bitsPerValue;
        for (size_t i = 0; i < numPoints; ++i) {
            if (hasBitmap) {
                size_t byteIndex = i / 8;
                size_t bitIndex = 7 - (i % 8);
                bool present  = (field.bitmap[byteIndex] >>bitIndex) & 1;
                if (!present) {
                    field.values[i] = std::numeric_limits<float>::quiet_NaN();
                    continue;
                }
            }
            uint64_t packed = 0;
            if (!br.readBits(bpv, packed)) {
                return false;
            }
            double val = (refVal + packed * bscale) * dscale;
            field.values[i] = static_cast<float>(val);
        }
        size_t bitsUsed = 32 + 8 + numPoints * bpv;
        size_t bitsTotal = static_cast<size_t>(secLen) * 8;
        if (bitsTotal > bitsUsed) {
            br.skipBits(static_cast<uint32_t>(bitsTotal - bitsUsed));
        }
        return true;
    }
}