#include "griblite/codec/grib1/GRIB1Reader.hpp"
#include "griblite/codec/grib1/GRIB1ParameterTable.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <utility>

namespace griblite {
    bool GRIB1Reader::isBitSet(const std::vector<uint8_t>& bitmap, size_t linearIndex, uint16_t unusedBits) noexcept {
        if (unusedBits > 7 || unusedBits > bitmap.size() * 8) return false;
        size_t totalBits = (bitmap.size() * 8) - unusedBits;
        if (linearIndex >= totalBits) return false;
        size_t byteIdx = linearIndex / 8;
        size_t bitIdx = 7 - (linearIndex % 8); 
        if (byteIdx >= bitmap.size()) return false; 
        return (bitmap[byteIdx] & (1 << bitIdx)) != 0;
    }
    template <typename T> int64_t GRIB1Reader::parseSignMagnitude(T rawValue, size_t numBits) noexcept {
        if (numBits == 0 || numBits > 64) {
            return 0;
        }
        uint64_t sanitized = static_cast<uint64_t>(rawValue);
        if (numBits < 64) {
            sanitized &= (1ULL << numBits) - 1ULL;
        }
        uint64_t signMask = 1ULL << (numBits - 1);
        uint64_t magnitude = sanitized & ~signMask;
        if (sanitized & signMask) {
            return -static_cast<int64_t>(magnitude);
        }
        return static_cast<int64_t>(magnitude);
    }
    float GRIB1Reader::convertIBMToIEEE(uint32_t IBMBits) noexcept {
        if (IBMBits == 0x00000000 || IBMBits == 0x80000000) {
            return 0.0f;
        }
        int32_t sign = (IBMBits & 0x80000000) ? -1 : 1;
        int32_t exponent = (IBMBits >> 24) & 0x7F;
        uint32_t mantissa = IBMBits & 0x00FFFFFF;
        if (mantissa == 0) {
            return 0.0f;
        }
        double value = static_cast<double>(mantissa) / 16777216.0; 
        value *= std::pow(16.0, exponent - 64);
        return static_cast<float>(sign * value);
    }
    std::optional<GribDataset> GRIB1Reader::parse(const uint8_t* data, size_t size) noexcept {
        if (size < 8) {
            return std::nullopt;
        }
        if (data[0] != 'G' || data[1] != 'R' || data[2] != 'I' || data[3] != 'B') {
            return std::nullopt;
        }
        uint32_t totalLength = (uint32_t(data[4]) << 16) |
            (uint32_t(data[5]) << 8)  |
            (uint32_t(data[6]));
        if (totalLength == 0) {
            totalLength = static_cast<uint32_t>(size);
        }
        if (totalLength > size) {
            return std::nullopt;
        }
        uint8_t edition = data[7];
        if (edition != 1) {
            return std::nullopt;
        }
        ByteBuffer buf(data, totalLength);
        BitReader br(buf);
        if (!br.skipBits(64)) {
            return std::nullopt;
        }
        GribField field;
        field.edition = edition;
        field.discipline = "0";
        uint16_t pdsLength = 0;
        bool hasGDS = false;
        bool hasBMS = false;
        std::cout << "Start";
        if (!parsePDS(br, field, hasGDS, hasBMS)) {
            return std::nullopt;
        }
        std::cout << "PDS Done";
        br.alignToByte();
        if (hasGDS){
            size_t startByte = br.getBitPosition() / 8;
            if (!parseGDS(br, field, totalLength, startByte)) {
                return std::nullopt;
            }
        }
        std::cout << "GDS Done";
        br.alignToByte();
        std::vector<uint8_t> bitmap;
        uint16_t unusedBits = 0;
        if (hasBMS) { 
            size_t startByte = br.getBitPosition() / 8;
            if (!parseBMS(br, bitmap, unusedBits, totalLength, startByte)) {
                return std::nullopt;
            }
        }
        br.alignToByte();
        std::cout << "BMS Done";
        size_t startByte = br.getBitPosition() / 8; 
        std::cout << "fail11";
        if (!parseBDS(br, field, bitmap, unusedBits, totalLength, startByte)) {
            std::cout << "failer";
            return std::nullopt;
        }
        std::cout << "BDS Done"; 
        GribDataset ds;
        ds.addField(std::move(field));
        return ds;
    }
    bool GRIB1Reader::parsePDS(BitReader& br, GribField& field, bool& hasGDS, bool& hasBMS) noexcept {
        size_t startBits = br.getBitPosition();
        uint64_t tmp = 0;
        auto read8 = [&](uint16_t& out) -> bool {
            if (!br.readBits(8, tmp)) {
                return false;
            }
            out = static_cast<uint16_t>(tmp);
            return true;
        };
        auto read16 = [&](uint32_t& out) -> bool {
            if (!br.readBits(16, tmp)) {
                return false;
            }
            out = static_cast<uint32_t>(tmp);
            return true;
        };
        // Octet 1-3
        if (!br.readBits(24, tmp)) {
            return false;
        }
        uint32_t pdsLength = static_cast<uint32_t>(tmp);
        std::cout << "pdsLength" << pdsLength;
        // Octet 4
        uint16_t tableVersion;
        if (!read8(tableVersion)) {
            return false;
        }
        std::cout << "tableVersion" << tableVersion;
        // Octet 5
        uint16_t centerId;
        if (!read8(centerId)) {
            return false;
        }
        // Octet 6
        uint16_t processId;
        if (!read8(processId)) {
            return false;
        }
        // Octet 7
        uint16_t gridId;
        if (!read8(gridId)) {
            return false;
        }
        // Octet 8
        uint16_t flags;
        if (!read8(flags)) {
            return false;
        }
        hasGDS = (flags & 0x80) != 0;
        hasBMS = (flags & 0x40) != 0;
        // Octet 9
        uint16_t parameterId;
        if (!read8(parameterId)) {
            return false;
        }
        // Octet 10
        uint16_t levelType;
        if (!read8(levelType)) {
            return false;
        }
        // Octets 11-12
        uint32_t levelValue;
        if (!read16(levelValue)) {
            return false;
        }
        // Octets 13-17
        uint16_t year;
        uint16_t month;
        uint16_t day;
        uint16_t hour;
        uint16_t minute;
        if (!read8(year) ||
            !read8(month) ||
            !read8(day) ||
            !read8(hour) ||
            !read8(minute))
        {
            return false;
        }
        // Octet 18
        uint16_t timeUnit;
        if (!read8(timeUnit)) {
            return false;
        }
        // Octets 19-20
        uint16_t P1;
        uint16_t P2;
        if (!read8(P1) || !read8(P2)) {
            return false;
        }
        // Octet 21
        uint16_t timeRange;
        if (!read8(timeRange)) {
            return false;
        }
        // Octets 22-23
        uint32_t numAverage;
        if (!read16(numAverage)) {
            return false;
        }
        // Octet 24
        uint16_t numMissing;
        if (!read8(numMissing)) {
            return false;
        }
        // Octet 25
        uint16_t century;
        if (!read8(century)) {
            return false;
        }
        // Octet 26
        uint16_t subCenter;
        if (!read8(subCenter)) {
            return false;
        }
        // Octets 27-28
        uint32_t rawDecimalScale;
        if (!read16(rawDecimalScale)) {
            return false;
        }
        int16_t decimalScale = static_cast<int16_t>(parseSignMagnitude(rawDecimalScale, 16));
        auto info = GRIB1ParameterTable::lookup(parameterId);
        field.parameter = info ? info->shortName : ("GRIB1_" + std::to_string(parameterId));
        field.levelType = std::to_string(levelType);
        field.levelValue = levelValue;
        field.decimalScale = decimalScale;
        if (timeUnit == 1) {
            field.forecastHours = P1;
        }
        size_t expectedEndBit = startBits + (pdsLength * 8);
        if (br.getBitPosition() < expectedEndBit) {
            if (!br.skipBits(expectedEndBit - br.getBitPosition())) {
                return false;
            }
        }
        return true;
    }
    bool GRIB1Reader::parseGDS(BitReader& br, GribField& field, size_t totalLength, size_t startByte) noexcept {
        constexpr double GRIB_MILLIDEGREE_SCALE = 1000.0;
        constexpr uint32_t MIN_GDS_LENGTH = 6;
        constexpr uint32_t REGULAR_LATLON_GDS_LENGTH = 32;
        constexpr uint8_t NO_VERTICAL_COORDINATES = 0;
        constexpr uint8_t NO_PV_LIST = 255;
        uint64_t tmp = 0;
        // Octets 1-3
        if (!br.readBits(24, tmp)) {
            return false;
        }
        const uint32_t gdsLength = static_cast<uint32_t>(tmp);
        if (gdsLength < 6 || (startByte + gdsLength) > totalLength) {
            return false;
        }
        std::cout << "gdsLength" << gdsLength;
        // Octet 4
        uint16_t nv;
        if (!br.readBits(8, tmp)) {
            return false;
        }
        nv = static_cast<uint16_t>(tmp);
        std::cout << "nv" << nv;
        // Octet 5
        uint16_t pv;
        if (!br.readBits(8, tmp)) {
            return false;
        }
        pv = static_cast<uint16_t>(tmp);
        std::cout << "pv" << pv;
        // Octet 6
        uint16_t gridType;
        if (!br.readBits(8, tmp)) {
            return false;
        }
        gridType = static_cast<uint16_t>(tmp); 
        std::cout << "GridType" << gridType;
        field.grid.gribGridType = gridType;
        switch (gridType) {
            case 0:
                field.grid.type = GridType::RegularLatLon;
                field.grid.supported = true;
                break;
            case 1:
                field.grid.type = GridType::Mercator;
                field.grid.supported = true;
                break;
            case 3:
                field.grid.type = GridType::LambertConformal;
                field.grid.supported = true;
                break;
            case 5:
                field.grid.type = GridType::PolarStereographic;
                field.grid.supported = true;
                break;
            case 10:
                field.grid.type = GridType::RotatedLatLon;
                field.grid.supported = false;
                break;
            default:
                field.grid.type = GridType::Unknown;
                field.grid.supported = false;
                break;
        }
        std::cout << "Supported" << field.grid.supported;
        switch (gridType) {
            case 0: {
                field.grid.supported = true;
                if (gdsLength < REGULAR_LATLON_GDS_LENGTH) {
                    return false;
                }
                // Unsupported vertical coordinate lists for now.
                if (nv != NO_VERTICAL_COORDINATES || pv != NO_PV_LIST) {
                    field.grid.supported = false;
                    size_t bytesRead = 6;
                    if (gdsLength > bytesRead) {
                        if (!br.skipBits((gdsLength - bytesRead) * 8)) {
                            return false;
                        }
                    }
                    return true;
                }
                // Octets 7-8
                uint64_t nx;
                if (!br.readBits(16, nx)) {
                    return false;
                }
                std::cout << "nx" << nx;
                // Octets 9-10
                uint64_t ny;
                if (!br.readBits(16, ny)) {
                    return false;
                }
                std::cout << "ny" << ny;
                field.grid.nx = static_cast<uint32_t>(nx);
                field.grid.ny = static_cast<uint32_t>(ny);
                // Octets 11-13
                uint64_t lat1;
                if (!br.readBits(24, lat1)) {
                    return false;
                }
                // Octets 14-16
                uint64_t lon1;
                if (!br.readBits(24, lon1)) {
                    return false;
                }
                // Octet 17
                uint64_t resolutionFlag;
                if (!br.readBits(8, resolutionFlag)) {
                    return false;
                }
                // Octets 18-20
                uint64_t lat2;
                if (!br.readBits(24, lat2)) {
                    return false;
                }
                // Octets 21-23
                uint64_t lon2;
                if (!br.readBits(24, lon2)) {
                    return false;
                }
                // Octets 24-25
                uint64_t dx;
                if (!br.readBits(16, dx)) {
                    return false;
                }
                // Octets 26-27
                uint64_t dy;
                if (!br.readBits(16, dy)) {
                    return false;
                }
                // Octet 28
                uint64_t scan;
                if (!br.readBits(8, scan)) {
                    return false;
                }
                field.grid.lat1 = static_cast<double>(parseSignMagnitude(lat1, 24)) / GRIB_MILLIDEGREE_SCALE;
                field.grid.lon1 = static_cast<double>(parseSignMagnitude(lon1, 24)) / GRIB_MILLIDEGREE_SCALE;
                field.grid.lat2 = static_cast<double>(parseSignMagnitude(lat2, 24)) / GRIB_MILLIDEGREE_SCALE;
                field.grid.lon2 = static_cast<double>(parseSignMagnitude(lon2, 24)) / GRIB_MILLIDEGREE_SCALE;
                field.grid.dx = static_cast<double>(dx) / GRIB_MILLIDEGREE_SCALE;
                field.grid.dy = static_cast<double>(dy) / GRIB_MILLIDEGREE_SCALE;
                // Resolution flag metadata
                field.grid.hasDirectionIncrements = (resolutionFlag & 0x80) != 0;
                field.grid.earthIsOblate = (resolutionFlag & 0x40) != 0;
                field.grid.vectorComponentsResolved = (resolutionFlag & 0x08) != 0;
                // Scan mode metadata (ON388 Table 8)
                field.grid.scanMode = static_cast<uint8_t>(scan);
                field.grid.iDirectionNegative = (scan & 0x80) != 0;
                field.grid.jDirectionPositive = (scan & 0x40) == 0;
                field.grid.adjacentPointsInJDirection = (scan & 0x20) != 0;
                break;
            }
            case 1: {
                if (gdsLength < 32) {
                    return false;
                }
                if (nv != NO_VERTICAL_COORDINATES || pv != NO_PV_LIST) {
                    field.grid.supported = false;
                    size_t bytesRead = 6;
                    if (gdsLength > bytesRead) {
                        if (!br.skipBits((gdsLength - bytesRead) * 8)) {
                            return false;
                        }
                    }
                    return true;
                }
                uint64_t nx, ny;
                uint64_t lat1, lon1;
                uint64_t resolutionFlag;
                uint64_t lat2, lon2;
                uint64_t latin;
                uint64_t scan;
                uint64_t dx, dy;
                if (!br.readBits(16, nx)) {
                    return false;
                }
                std::cout << "nx" << nx;
                if (!br.readBits(16, ny)) {
                    return false;
                }
                std::cout << "ny" << ny;
                if (!br.readBits(24, lat1)) {
                    return false;
                }
                if (!br.readBits(24, lon1)) {
                    return false;
                }
                if (!br.readBits(8, resolutionFlag)) {
                    return false;
                }
                if (!br.readBits(24, lat2)) {
                    return false;
                }
                if (!br.readBits(24, lon2)) {
                    return false;
                }
                if (!br.readBits(16, latin)) {
                    return false;
                }
                if (!br.readBits(8, scan)) {
                    return false;
                }
                if (!br.readBits(24, dx)) {
                    return false;
                }
                if (!br.readBits(24, dy)) {
                    return false;
                }
                field.grid.nx = static_cast<uint32_t>(nx);
                field.grid.ny = static_cast<uint32_t>(ny);
                field.grid.lat1 = static_cast<double>(parseSignMagnitude(lat1, 24)) / GRIB_MILLIDEGREE_SCALE;
                field.grid.lon1 = static_cast<double>(parseSignMagnitude(lon1, 24)) / GRIB_MILLIDEGREE_SCALE;
                field.grid.lat2 = static_cast<double>(parseSignMagnitude(lat2, 24)) / GRIB_MILLIDEGREE_SCALE;
                field.grid.lon2 = static_cast<double>(parseSignMagnitude(lon2, 24)) / GRIB_MILLIDEGREE_SCALE;
                // Mercator standard latitude
                field.grid.latin = static_cast<double>(parseSignMagnitude(latin, 16))  / GRIB_MILLIDEGREE_SCALE;
                // Mercator spacing is encoded in metres
                field.grid.dx = static_cast<double>(dx);
                field.grid.dy = static_cast<double>(dy);
                field.grid.hasDirectionIncrements = (resolutionFlag & 0x80) != 0;
                field.grid.earthIsOblate = (resolutionFlag & 0x40) != 0;
                field.grid.vectorComponentsResolved = (resolutionFlag & 0x08) != 0;
                field.grid.scanMode = static_cast<uint8_t>(scan);
                field.grid.iDirectionNegative = (scan & 0x80) != 0;
                field.grid.jDirectionPositive = (scan & 0x40) == 0;
                field.grid.adjacentPointsInJDirection = (scan & 0x20) != 0;
                break;
            }
            case 3: {
                if (gdsLength < 32) {
                    return false;
                }
                if (nv != NO_VERTICAL_COORDINATES || pv != NO_PV_LIST) {
                    field.grid.supported = false;
                    size_t bytesRead = 6;
                    if (gdsLength > bytesRead) {
                        if (!br.skipBits((gdsLength - bytesRead) * 8)) {
                            return false;
                        }
                    }
                    return true;
                }
                uint64_t nx, ny;
                uint64_t lat1, lon1;
                uint64_t resolutionFlag;
                uint64_t latin1, latin2;
                uint64_t lov;
                uint64_t scan;
                uint64_t dx, dy;
                uint64_t projCenter;
                uint64_t latSP, lonSP;
                if (!br.readBits(16, nx)) {
                    return false;
                }
                std::cout << "nx" << nx;
                if (!br.readBits(16, ny)) {
                    return false;
                }
                std::cout << "ny" << ny;
                if (!br.readBits(24, lat1)) {
                    return false;
                }
                if (!br.readBits(24, lon1)) {
                    return false;
                }
                if (!br.readBits(8, resolutionFlag)) {
                    return false;
                }
                if (!br.readBits(24, lov)) {
                    return false;
                }
                if (!br.readBits(24, dx)) {
                    return false;
                }
                if (!br.readBits(24, dy)) {
                    return false;
                }
                if(!br.readBits(8, projCenter)) {
                    return false;
                }
                if(!br.readBits(24, latin1)) {
                    return false;
                }                
                if(!br.readBits(24, latin2)) {
                    return false;
                }
                if (!br.readBits(24, latSP)) {
                    return false;
                }
                if(!br.readBits(24, lonSP)) {
                    return false;
                }
                if(!br.readBits(8, scan)) {
                    return false;
                } 
                field.grid.nx = static_cast<uint32_t>(nx);
                field.grid.ny = static_cast<uint32_t>(ny);
                field.grid.lat1 = parseSignMagnitude(lat1, 24) / 1000.0;
                field.grid.lon1 = parseSignMagnitude(lon1, 24) / 1000.0;
                field.grid.lov = parseSignMagnitude(lov, 24) / 1000.0;
                field.grid.dx = dx; // meters
                field.grid.dy = dy; // meters
                field.grid.latin1 = parseSignMagnitude(latin1, 24) / 1000.0;
                field.grid.latin2 = parseSignMagnitude(latin2, 24) / 1000.0;
                field.grid.latSP = parseSignMagnitude(latSP, 24) / 1000.0;
                field.grid.lonSP = parseSignMagnitude(lonSP, 24) / 1000.0;
                field.grid.projectionSouth = (projCenter & 0x80) != 0;
                field.grid.scanMode = scan;
                field.grid.iDirectionNegative = (scan & 0x80) != 0;
                field.grid.jDirectionPositive = (scan & 0x40) == 0;
                field.grid.adjacentPointsInJDirection = (scan & 0x20) != 0;
                break;
            }                  
            case 5: {
                if (gdsLength < 32) {
                    return false;
                }
                if (nv != NO_VERTICAL_COORDINATES || pv != NO_PV_LIST) {
                    field.grid.supported = false;
                    size_t bytesRead = 6;
                    if (gdsLength > bytesRead) {
                        if (!br.skipBits((gdsLength - bytesRead) * 8)) {
                            return false;
                        }
                    }
                    return true;
                }
                uint64_t nx, ny;
                uint64_t lat1, lon1;
                uint64_t resolutionFlag;
                uint64_t lov;
                uint64_t projCenter;
                uint64_t scan;
                uint64_t dx, dy;
                if (!br.readBits(16, nx)) {
                    return false;
                }
                if (!br.readBits(16, ny)) {
                    return false;
                }
                if (!br.readBits(24, lat1)) {
                    return false;
                }
                if (!br.readBits(24, lon1)) {
                    return false;
                }
                if (!br.readBits(8, resolutionFlag)) {
                    return false;
                }
                if (!br.readBits(24, lov)) {
                    return false;
                }
                if (!br.readBits(24, dx)) {
                    return false;
                }
                if (!br.readBits(24, dy)) {
                    return false;
                }
                if(!br.readBits(8, projCenter)) {
                    return false;
                }
                if(!br.readBits(8, scan)) {
                    return false;
                } 
                field.grid.nx = static_cast<uint32_t>(nx);
                field.grid.ny = static_cast<uint32_t>(ny);
                field.grid.lat1 = parseSignMagnitude(lat1, 24) / 1000.0;
                field.grid.lon1 = parseSignMagnitude(lon1, 24) / 1000.0;
                field.grid.lov = parseSignMagnitude(lov, 24) / 1000.0;
                field.grid.dx = dx; // meters
                field.grid.dy = dy; // meters
                field.grid.projectionSouth = (projCenter & 0x80) != 0;
                field.grid.scanMode = scan;
                field.grid.iDirectionNegative = (scan & 0x80) != 0;
                field.grid.jDirectionPositive = (scan & 0x40) == 0;
                field.grid.adjacentPointsInJDirection = (scan & 0x20) != 0;
                break;
            }
            default:
                return false;
        }
        std::cout << "Done";
        size_t expectedEndBit = (startByte + gdsLength) * 8;
        if (br.getBitPosition() < expectedEndBit) {
            if (!br.skipBits(expectedEndBit - br.getBitPosition())) {
                return false;
            }
        }
        return true;
    }
    bool GRIB1Reader::parseBMS(BitReader& br, std::vector<uint8_t>& bitmap, uint16_t& unusedBits, size_t totalLength, size_t startByte) noexcept {
        uint64_t tmp = 0;
        // Octets 1-3
        if (!br.readBits(24, tmp)) {
            return false;
        }
        const uint32_t bmsLength = static_cast<uint32_t>(tmp);
        if (bmsLength < 6 || startByte > totalLength || bmsLength > totalLength - startByte) {
            return false;
        }
        // Octet 4
        if (!br.readBits(8, tmp)) {
            return false;
        }
        unusedBits = static_cast<uint16_t>(tmp);
        if (unusedBits > 7) {
            return false;
        }
        // Octets 5-6
        uint16_t bitmapIndicator;
        if (!br.readBits(16, tmp)) {
            return false;
        }
        bitmapIndicator = static_cast<uint32_t>(tmp);
        // explicit bitmap 
        if (bitmapIndicator != 0) {
            return false;
        }
        const size_t bitmapBytes = bmsLength - 6;
        if ((bitmapBytes * 8) < unusedBits) {
            return false;
        }
        bitmap.resize(bitmapBytes);
        for (size_t i = 0; i < bitmapBytes; ++i) {
            if (!br.readBits(8, tmp)) {
                return false;
            }
            bitmap[i] = static_cast<uint8_t>(tmp);
        }
        size_t expectedEndBit = (startByte + bmsLength) * 8;
        if (br.getBitPosition() > expectedEndBit) {
            return false;
        }
        if (br.getBitPosition() < expectedEndBit) {
            if (!br.skipBits(expectedEndBit - br.getBitPosition())) {
                return false;
            }
        }
        return true;
    }
    bool GRIB1Reader::parseBDS(BitReader& br, GribField& field, const std::vector<uint8_t>& bitmap, uint16_t unusedBitsInBMS, size_t totalLength, size_t startByte) noexcept {
        uint64_t tmp = 0;
        std::cout << "startbdsparse\n";
        if (startByte > totalLength || br.getBitPosition() != startByte * 8) {
            std::cout << "fail1";
            return false;
        }
        // OCTETS 1–3
        if (!br.readBits(24, tmp)) {
            std::cout << "fail2";
            return false;
        }
        std::cout << "Octet 1-3";
        const uint32_t bdsLength = static_cast<uint32_t>(tmp);
        std::cout << "bdsLength" << bdsLength;
        if (bdsLength < 12 || startByte > totalLength || bdsLength > totalLength - startByte) {
            std::cout << (bdsLength < 12);
            std::cout << (bdsLength > totalLength - startByte);
            return false;
        }
        std::cout << "here";
        const size_t sectionStartBits = br.getBitPosition() - 24;
        const size_t sectionBits = bdsLength * 8;
        const size_t sectionEndBit = (startByte + bdsLength) * 8;
        // OCTET 4
        if (!br.readBits(8, tmp)) {
            std::cout << "Failure point";
            return false;
        }

        uint16_t flagByte = static_cast<uint8_t>(tmp);
        const bool sphericalHarmonics = (flagByte & 0x80) != 0;
        const bool complexPacking = (flagByte & 0x40) != 0;
        const bool integerData = (flagByte & 0x20) != 0;
        const bool hasAdditionalFlags = (flagByte & 0x10) != 0;
        uint16_t packingType = complexPacking;
        const uint8_t unusedBits = flagByte & 0x0F;
        std::cout << "Got here";
        if (sphericalHarmonics) {
            return false;
        }
        std::cout << "Passed Octet 4";
        // OCTETS 5–6
        if (!br.readBits(16, tmp)) {
            return false;
        }
        int16_t binaryScale = parseSignMagnitude(tmp, 16);
        // OCTETS 7–10: REFERENCE VALUE (IBM FLOAT)
        if (!br.readBits(32, tmp)) {
            return false;
        }
        float refValue = convertIBMToIEEE(static_cast<uint32_t>(tmp));
        if (!std::isfinite(refValue)) {
            return false;
        }
        // OCTET 11
        if (!br.readBits(8, tmp)) {
            return false;
        }
        uint8_t bitsPerValue = static_cast<uint8_t>(tmp);
        if (bitsPerValue > 64) {
            return false;
        }
        const size_t nPoints = field.grid.nx * field.grid.ny;
        auto isValid = [&](size_t i) -> bool {
            if (bitmap.empty()) return true;
            return isBitSet(bitmap, i, unusedBitsInBMS);
        };
        size_t validCount = 0;
        for (size_t i = 0; i < nPoints; ++i) {
            if (isValid(i)) ++validCount;
        }
        const double bscale = std::pow(2.0, binaryScale);
        const double dscale = std::pow(10.0, -field.decimalScale);
        field.values.assign(nPoints, std::numeric_limits<float>::quiet_NaN());
        // SIMPLE PACKING
        if (packingType == 0) {
            std::cout << "simple unpacking";
            if (bitsPerValue == 0) {
                for (size_t i = 0; i < nPoints; ++i) {
                    if (isValid(i)) {
                        field.values[i] = refValue * dscale;
                    }
                }
                return true;
            }
            const size_t dataBits = (bdsLength - 11) * 8 - unusedBits;
            if (validCount * bitsPerValue > dataBits) {
                return false;
            }
            for (size_t i = 0; i < nPoints; ++i) {
                if (!isValid(i)) {
                    continue;
                }
                uint64_t packed = 0;
                if (!br.readBits(bitsPerValue, packed)) {
                    return false;
                }
                double val = (refValue + packed * bscale) * dscale;
                field.values[i] = static_cast<float>(val);
            }
            return true;
        }
        // COMPLEX PACKING 
        // COMPLEX PACKING (custom GribGenerator grouped-residual format)
        if (packingType == 1) {
            std::cout << "compelx unpacking";
            uint64_t n1 = 0;
            uint64_t extFlags = 0;
            uint64_t n2 = 0;
            uint64_t nFirst = 0;
            uint64_t nSecond = 0;
            uint64_t reserved = 0;
            // octets 12-21.
            if (!br.readBits(16, n1) ||
                !br.readBits(8, extFlags) ||
                !br.readBits(16, n2) ||
                !br.readBits(16, nFirst) ||
                !br.readBits(16, nSecond) ||
                !br.readBits(8, reserved)) {
                return false;
            }
            const size_t P1 = static_cast<size_t>(nFirst);
            const size_t P2 = static_cast<size_t>(nSecond);
            if (P1 == 0 || P2 == 0 || bitsPerValue == 0) {
                return false;
            }
            (void)extFlags;
            (void)reserved;
            if (n1 < 22 || n2 <= n1 ||
                n1 > bdsLength || n2 > bdsLength) {
                return false;
            }
            const size_t n1Byte = startByte + static_cast<size_t>(n1) - 1;
            const size_t n2Byte = startByte + static_cast<size_t>(n2) - 1;
            const size_t endByte = startByte + bdsLength;
            if (n1Byte > endByte || n2Byte > endByte) {
                return false;
            }
            const size_t bitmapBytes = (P2 + 7) / 8;
            const size_t expectedHeaderBytes = P1 + bitmapBytes;
            if (n1Byte - (startByte + 21) != expectedHeaderBytes) {
                return false;
            }
            std::vector<uint8_t> groupWidths(P1);
            for (size_t g = 0; g < P1; ++g) {
                if (!br.readBits(8, tmp)) {
                    return false;
                }
                groupWidths[g] = static_cast<uint8_t>(tmp);
                if (groupWidths[g] > 64) {
                    return false;
                }
            }
            std::vector<uint8_t> secondaryBitmap(bitmapBytes);
            for (size_t i = 0; i < bitmapBytes; ++i) {
                if (!br.readBits(8, tmp)) {
                    return false;
                }
                secondaryBitmap[i] = static_cast<uint8_t>(tmp);
            }
            if (br.getBitPosition() != n1Byte * 8) {
                return false;
            }
            std::vector<size_t> groupSizes;
            groupSizes.reserve(P1);
            size_t currentGroupSize = 0;
            size_t groupCount = 0;
            for (size_t i = 0; i < P2; ++i) {
                const bool groupStart =
                    (secondaryBitmap[i / 8] >> (7 - (i % 8))) & 1;
                if (groupStart) {
                    if (groupCount > 0) {
                        groupSizes.push_back(currentGroupSize);
                    }
                    ++groupCount;
                    currentGroupSize = 1;
                } else {
                    if (groupCount == 0) {
                        return false;
                    }
                    ++currentGroupSize;
                }
            }
            if (groupCount != P1) {
                return false;
            }
            groupSizes.push_back(currentGroupSize);
            const size_t firstOrderBits = P1 * bitsPerValue;
            const size_t firstOrderBytes = (firstOrderBits + 7) / 8;
            if (n2Byte - n1Byte != firstOrderBytes) {
                return false;
            }
            std::vector<uint64_t> firstOrderValues(P1);
            for (size_t g = 0; g < P1; ++g) {
                if (!br.readBits(bitsPerValue, firstOrderValues[g])) {
                    return false;
                }
            }
            if (br.getBitPosition() > n2Byte * 8) {
                return false;
            }
            br.setBitPosition(n2Byte * 8);
            size_t secondOrderBits = 0;
            for (size_t g = 0; g < P1; ++g) {
                const size_t width = groupWidths[g];
                if (width != 0 &&
                    groupSizes[g] > (SIZE_MAX - secondOrderBits) / width) {
                    return false;
                }
                secondOrderBits += groupSizes[g] * width;
            }
            const size_t secondOrderBytes = (secondOrderBits + 7) / 8;
            if (endByte - n2Byte != secondOrderBytes) {
                return false;
            }
            std::vector<uint64_t> packedValues;
            packedValues.reserve(P2);
            for (size_t g = 0; g < P1; ++g) {
                const uint64_t firstOrder = firstOrderValues[g];
                const uint8_t width = groupWidths[g];
                for (size_t k = 0; k < groupSizes[g]; ++k) {
                    uint64_t residual = 0;
                    if (width > 0 && !br.readBits(width, residual)) {
                        return false;
                    }
                    if (residual > UINT64_MAX - firstOrder) {
                        return false;
                    }
                    packedValues.push_back(firstOrder + residual);
                }
            }
            if (packedValues.size() != P2) {
                return false;
            }
            if (P2 != validCount) {
                return false;
            }
            size_t valueIndex = 0;
            for (size_t i = 0; i < nPoints; ++i) {
                if (!isValid(i)) {
                    continue;
                }
                const double val =
                    (static_cast<double>(refValue) +
                    static_cast<double>(packedValues[valueIndex++]) * bscale)
                    * dscale;
                field.values[i] = static_cast<float>(val);
            }
            return true;
        }
        return false;
    }
}