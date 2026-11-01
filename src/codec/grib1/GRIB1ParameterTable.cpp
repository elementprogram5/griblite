#include "griblite/codec/grib1/GRIB1ParameterTable.hpp"

namespace griblite {
    std::optional<GRIB1ParamInfo> GRIB1ParameterTable::lookup(int id) noexcept {
        switch(id) {
            case 1:  
                return GRIB1ParamInfo{"PRES", "Pressure", "Pa"};
            case 2:  
                return GRIB1ParamInfo{"PRMSL", "Pressure reduced to MSL", "Pa"};
            case 3:  
                return GRIB1ParamInfo{"PTEND", "Pressure tendency", "Pa/s"};
            case 4:  
                return GRIB1ParamInfo{"PVORT", "Potential vorticity", "K*m^2/kg*s"};
            case 5:  
                return GRIB1ParamInfo{"ICAHT", "ICAO Standard Atmosphere Reference Height", "m"};
            case 6:  
                return GRIB1ParamInfo{"GP",    "Geopotential", "m^2/s^2"};
            case 7:  
                return GRIB1ParamInfo{"HGT", "Geopotential height", "gpm"};
            case 8:  
                return GRIB1ParamInfo{"DIST", "Geometrical height", "m"};
            case 9:  
                return GRIB1ParamInfo{"HSTDV", "Standard deviation of height", "m"};
            case 10: 
                return GRIB1ParamInfo{"TOZNE", "Total ozone", "Dobson"};
            case 11: 
                return GRIB1ParamInfo{"TMP", "Temperature", "K"};
            case 12: 
                return GRIB1ParamInfo{"VTMP", "Virtual temperature", "K"};
            case 13: 
                return GRIB1ParamInfo{"POT", "Potential temperature", "K"};
            case 14: 
                return GRIB1ParamInfo{"EPOT", "Pseudo-adiabatic potential temperature", "K"};
            case 15: 
                return GRIB1ParamInfo{"TMAX", "Maximum temperature", "K"};
            case 16: 
                return GRIB1ParamInfo{"TMIN", "Minimum temperature", "K"};
            case 17: 
                return GRIB1ParamInfo{"DPT", "Dew-point temperature", "K"};
            case 18: 
                return GRIB1ParamInfo{"DEPR", "Dew-point depression (or deficit)", "K"};
            case 19: 
                return GRIB1ParamInfo{"LAPR",  "Lapse rate", "K/m"};
            case 20: 
                return GRIB1ParamInfo{"VIS",   "Visibility", "m"};
            case 24: 
                return GRIB1ParamInfo{"PLI",   "Parcel lifted index (to 500 hPa)", "K"};
            case 25: 
                return GRIB1ParamInfo{"TMPA",  "Temperature anomaly", "K"};
            case 26: 
                return GRIB1ParamInfo{"PRESA", "Pressure anomaly", "Pa"};
            case 27: 
                return GRIB1ParamInfo{"GPA",   "Geopotential height anomaly", "gpm"};
            case 31: 
                return GRIB1ParamInfo{"WDIR", "Wind direction", "deg"};
            case 32: 
                return GRIB1ParamInfo{"WIND", "Wind speed", "m/s"};
            case 33: 
                return GRIB1ParamInfo{"UGRD", "U-component of wind", "m/s"};
            case 34: 
                return GRIB1ParamInfo{"VGRD", "V-component of wind", "m/s"};
            case 35: 
                return GRIB1ParamInfo{"STRM",  "Stream function", "m^2/s"};
            case 36: 
                return GRIB1ParamInfo{"VPOT",  "Velocity potential", "m^2/s"};
            case 37: 
                return GRIB1ParamInfo{"MNTSF", "Montgomery stream function", "m^2/s^2"};
            case 38: 
                return GRIB1ParamInfo{"SGCVV", "Sigma coordinate vertical velocity", "/s"};
            case 39: 
                return GRIB1ParamInfo{"VVEL",  "Vertical velocity (pressure)", "Pa/s"};
            case 40: 
                return GRIB1ParamInfo{"DZDT",  "Vertical velocity (geometric)", "m/s"};
            case 41: 
                return GRIB1ParamInfo{"ABSV",  "Absolute vorticity", "/s"};
            case 42: 
                return GRIB1ParamInfo{"ABSD",  "Absolute divergence", "/s"};
            case 43: 
                return GRIB1ParamInfo{"RELV",  "Relative vorticity", "/s"};
            case 44: 
                return GRIB1ParamInfo{"RELD",  "Relative divergence", "/s"};
            case 45: 
                return GRIB1ParamInfo{"VUSHR",  "Vertical u-component wind shear", "/s"};
            case 46: 
                return GRIB1ParamInfo{"VVSHR",  "Vertical v-component wind shear", "/s"};
            case 47: 
                return GRIB1ParamInfo{"CSTX",  "Current direction", "deg true"};
            case 48: 
                return GRIB1ParamInfo{"CSTY",  "Current speed", "m/s"};
            case 49: 
                return GRIB1ParamInfo{"UOGRD", "u-component of current", "m/s"};
            case 50: 
                return GRIB1ParamInfo{"VOGRD", "v-component of current", "m/s"};
            case 51: 
                return GRIB1ParamInfo{"SPFH", "Specific humidity", "kg/kg"};
            case 52: 
                return GRIB1ParamInfo{"RH", "Relative humidity", "%"};
            case 53: 
                return GRIB1ParamInfo{"MIXR", "Humidity mixing ratio", "kg/kg"};
            case 54:    
                return GRIB1ParamInfo{"PWAT", "Precipitable water", "kg/m^2"};
            case 55: 
                return GRIB1ParamInfo{"VAPP", "Vapour pressure", "Pa"};
            case 56: 
                return GRIB1ParamInfo{"SATD", "Saturation deficit", "Pa"};
            case 57: 
                return GRIB1ParamInfo{"EVP", "Evaporation", "kg/m^2"};
            case 58: 
                return GRIB1ParamInfo{"CICE",  "Cloud ice", "kg/m^2"};
            case 59: 
                return GRIB1ParamInfo{"PRATE", "Precipitation rate", "kg/m^2/s"};
            case 60: 
                return GRIB1ParamInfo{"TSTM",  "Thunderstorm probability", "%"};
            case 61: 
                return GRIB1ParamInfo{"APCP",  "Total precipitation", "kg/m^2"};
            case 62: 
                return GRIB1ParamInfo{"NCPCP", "Large scale precipitation", "kg/m^2"};
            case 63: 
                return GRIB1ParamInfo{"ACPCP", "Convective precipitation", "kg/m^2"};
            case 64: 
                return GRIB1ParamInfo{"SRWEQ", "Snowfall rate water equivalent", "kg/m^2/s"};
            case 65: 
                return GRIB1ParamInfo{"WEASD", "Water equiv. of accum. snow depth", "kg/m^2"};
            case 66: 
                return GRIB1ParamInfo{"SNOD",  "Snow depth", "m"};
            case 67: 
                return GRIB1ParamInfo{"MIXHT", "Mixed layer depth", "m"};
            case 68: 
                return GRIB1ParamInfo{"TTHDP",  "Transient thermocline depth", "m"};
            case 69: 
                return GRIB1ParamInfo{"MHTD",  "Main thermocline depth", "m"};
            case 70: 
                return GRIB1ParamInfo{"MHTA",  "Main thermocline anomaly", "m"};
            case 71: 
                return GRIB1ParamInfo{"TCDC", "Total cloud cover", "%"};
            case 72: 
                return GRIB1ParamInfo{"CDCON", "Convective cloud cover", "%"};
            case 73: 
                return GRIB1ParamInfo{"LCDC", "Low cloud cover", "%"};
            case 74: 
                return GRIB1ParamInfo{"MCDC", "Medium cloud cover", "%"};
            case 75:
                return GRIB1ParamInfo{"HCDC", "High cloud cover", "%"};
            case 76: 
                return GRIB1ParamInfo{"CWAT", "Cloud water", "kg/m^2"};
            default:
                break;
        };
        return std::nullopt;
    }
}