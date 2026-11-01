#include "griblite/codec/grib2/GRIB2ParameterTable.hpp"

namespace griblite {
    std::optional<GRIB2ParamInfo> GRIB2ParameterTable::lookup(
        int discipline,
        int category,
        int number) noexcept
    {
        if (discipline == 0) {
            if (category == 0) {
                switch (number) {
                    case 0:
                        return GRIB2ParamInfo{"TMP", "Temperature", "K"};
                    case 1:
                        return GRIB2ParamInfo{"VTMP", "Virtual temperature", "K"};
                    case 2:
                        return GRIB2ParamInfo{"POT", "Potential Temperature", "K"};
                    case 3:
                        return GRIB2ParamInfo{"EPOT", "Equivalent Potential Temperature", "K"};
                    case 4:
                        return GRIB2ParamInfo{"TMAX", "Maximum Temperature", "K"};
                    case 5:
                        return GRIB2ParamInfo{"TMIN", "Minimum Temperature", "K"};
                    case 6:
                        return GRIB2ParamInfo{"DPT", "Dew Point Temperature", "K"};
                    case 7:
                        return GRIB2ParamInfo{"DEPR", "Dew Point Depression", "K"};
                }
            }
            if (category == 1) {
                switch (number) {
                    case 0:
                        return GRIB2ParamInfo{"SPFH", "Specific humidity", "kg/kg"};
                    case 1:
                        return GRIB2ParamInfo{"RH", "Relative humidity", "%"};
                    case 6:
                        return GRIB2ParamInfo{"PWAT", "Percipitable water", "kg/m^2"};
                    case 7:
                        return GRIB2ParamInfo{"VAPP", "Vapor Pressure", "Pa"};
                    case 8:
                        return GRIB2ParamInfo{"SATD", "Saturation deficit", "Pa"};
                    case 2:
                        return GRIB2ParamInfo{"EVP", "Evaporation", "kg/m^2"};
                }
            }
            if (category == 2) {
                switch (number) {
                    case 0:
                        return GRIB2ParamInfo{"WIND", "Wind speed", "m/s"};
                    case 1:
                        return GRIB2ParamInfo{"WDIR", "Wind direction", "deg"};
                    case 2:
                        return GRIB2ParamInfo{"UGRD", "U-component of wind", "m/s"};
                    case 3:
                        return GRIB2ParamInfo{"VGRD", "V-component of wind", "m/s"};
                    case 8:
                        return GRIB2ParamInfo{"GUST", "Wind Gust", "m/s"};
                    case 9:
                        return GRIB2ParamInfo{"LFTX", "Surface Lifted Index", "K"};
                }
            }
            if (category == 3) {
                switch (number) {
                    case 0:
                        return GRIB2ParamInfo{"PRES", "Pressure", "Pa"};
                    case 1:
                        return GRIB2ParamInfo{"PRMSL", "Mean sea level pressure", "Pa"};
                    case 5:
                        return GRIB2ParamInfo{"HGT", "Geopotential Height", "gpm"};
                    case 6:
                        return GRIB2ParamInfo{"DIST", "Geometric height", "m"};
                    case 7:
                        return GRIB2ParamInfo{"HSTDV", "Standard deviation of height", "m"}; 
                }
            }
            if (category == 4) {
                switch (number) {
                    case 0:
                        return GRIB2ParamInfo{"SWDOWN", "Downward Shortwave radiation", "W/m^2"};
                    case 1:
                        return GRIB2ParamInfo{"SWUP", "Upward Shortwave radiation", "W/m^2"};
                }
            }
            if (category == 5) {
                switch (number) {
                    case 0:
                        return GRIB2ParamInfo{"LWDOWN", "Downward Longwave radiation", "W/m^2"};
                    case 1:
                        return GRIB2ParamInfo{"LWUP", "Upward Longwave radiation", "W/m^2"};
                }
            }
            if (category == 6) {
                switch (number) {
                    case 1:
                        return GRIB2ParamInfo{"TCDC", "Total cloud Cover", "%"};
                    case 2:
                        return GRIB2ParamInfo{"CDCON", "Convective cloud Cover", "%"};
                    case 3:
                        return GRIB2ParamInfo{"LCDC", "Low cloud Cover", "%"};
                    case 4:
                        return GRIB2ParamInfo{"MCDC", "Mid cloud Cover", "%"};
                    case 5:
                        return GRIB2ParamInfo{"HCDC", "High cloud Cover", "%"};
                }
            }
            if (category == 7) {
                switch (number) {
                    case 0:
                        return GRIB2ParamInfo{"PRATE", "Percipitation rate", "kg/m^2/s"};
                    case 1:
                        return GRIB2ParamInfo{"TP", "Total precipitation", "kg/m^2"};
                    case 5:
                        return GRIB2ParamInfo{"ACPCP", "Convective precipitation", "kg/m^2"};
                    case 6:
                        return GRIB2ParamInfo{"NCPCP", "Non-convective precipitation", "kg/m^2"};
                }
            }
            if (category == 14) {
                switch (number) {
                    case 0:
                        return GRIB2ParamInfo{"SOILM", "Soil moisture content", "kg/m^2"};
                    case 1:
                        return GRIB2ParamInfo{"SOILT", "Soil temperature", "k"};
                }
            }
            if (category == 19) {
                switch (number) {
                    case 0:
                        return GRIB2ParamInfo{"TKE", "Turbulent kinetic energy", "J/kg"};
                }
            }
        }
        return std::nullopt;
    }
}