#pragma once
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include "GribField.hpp"

namespace griblite {
    struct FieldKey {
        std::string parameter;
        std::string levelType;
        double levelValue;
        int forecastHour;
        bool operator==(const FieldKey& other) const noexcept {
            return parameter == other.parameter &&
                levelType == other.levelType &&
                levelValue == other.levelValue &&
                forecastHour == other.forecastHour;
        }
    };
    struct FieldKeyHash {
        size_t operator()(const FieldKey& k) const noexcept {
            size_t h1 = std::hash<std::string>{}(k.parameter);
            size_t h2 = std::hash<std::string>{}(k.levelType);
            size_t h3 = std::hash<double>{}(k.levelValue);
            size_t h4 = std::hash<int>{}(k.forecastHour);
            return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1) ^(h4 << 2);
        }
    };
    class GribDataset {
        public:
            void addField(GribField field);
            const std::vector<GribField>& fields() const noexcept { return m_fields; }
            const GribField* findField(const std::string& parameter, 
                const std::string& levelType, 
                double levelValue, 
                int forecastHour = 0) const noexcept;
        private:
            std::vector<GribField> m_fields;
            std::unordered_map<FieldKey, size_t, FieldKeyHash> m_index;
    };
}