#include "griblite/model/GribDataset.hpp"

namespace griblite {
    void GribDataset::addField(GribField field) {
        FieldKey key {
            field.parameter,
            field.levelType,
            field.levelValue,
            field.forecastHours
        };
        size_t index = m_fields.size();
        m_fields.push_back(std::move(field));
        m_index[key] = index;
    }
    const GribField* GribDataset::findField(const std::string& parameter,
        const std::string& levelType,
        double levelValue,
        int forecastHour) const noexcept 
    {
        FieldKey key { 
            parameter,
            levelType,
            levelValue,
            forecastHour
        };
        auto it = m_index.find(key);
        if (it == m_index.end()) {
            return nullptr;
        }
        return &m_fields[it->second];
    } 
}