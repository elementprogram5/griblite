#pragma once
#include <string>
#include <optional>

namespace griblite {
    struct GRIB1ParamInfo {
        std::string shortName;
        std::string longName;
        std::string units;
    };
    class GRIB1ParameterTable {
        public:
            static std::optional<GRIB1ParamInfo> lookup(int id) noexcept;
    };
}