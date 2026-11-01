#pragma once
#include <string>
#include <optional>

namespace griblite {
    struct GRIB2ParamInfo {
        std::string shortName;
        std::string longName;
        std::string units;
    };
    class GRIB2ParameterTable {
        public:
            static std::optional<GRIB2ParamInfo> lookup(
                int discipline,
                int category,
                int number) noexcept;
    };
}