
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

#include "griblite/GribLite.hpp"


std::vector<double> loadReferenceValues(const std::string& path) {
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error("Could not open reference file: " + path);
    }

    std::vector<double> values;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        values.push_back(std::stod(line));
    }

    return values;
}

bool compareValues(
    const griblite::GribField& field,
    const std::vector<double>& reference
) {
    if (field.values.size() != reference.size()) {
        std::cout << "Value count mismatch!\n"
                  << "Decoded: " << field.values.size() << "\n"
                  << "Reference: " << reference.size() << "\n";
        return false;
    }

    // Calculate the maximum expected packing error.
    double packingStep =
        std::ldexp(1.0, field.binaryScale) *
        std::pow(10.0, -field.decimalScale);

    double tolerance = std::abs(packingStep) * 0.5;

    // Allow a small additional margin for floating-point arithmetic.
    size_t matched = 0;
    size_t mismatched = 0;
    size_t missingMatched = 0;
    size_t missingMismatched = 0;

    double maximumError = 0.0;

    for (size_t i = 0; i < reference.size(); ++i) {
        double expected = reference[i];
        double actual = field.values[i];

        bool expectedMissing = !std::isfinite(expected);
        bool actualMissing = !std::isfinite(actual);

        if (expectedMissing || actualMissing) {
            if (expectedMissing && actualMissing) {
                missingMatched++;
            } else {
                missingMismatched++;
                mismatched++;
            }

            continue;
        }

        double error = std::abs(expected - actual);

        maximumError = std::max(maximumError, error);

        double allowedError = tolerance +
            8.0 * std::numeric_limits<float>::epsilon() *
            std::max({1.0, std::abs(expected), std::abs(actual)});

        if (error <= allowedError) {
            matched++;
        } else {
            mismatched++;
        }
    }

    std::cout << "\n--- Numerical Comparison ---\n";
    std::cout << "Reference values: " << reference.size() << "\n";
    std::cout << "Matched values: " << matched << "\n";
    std::cout << "Mismatched values: " << mismatched << "\n";
    std::cout << "Matching missing values: " << missingMatched << "\n";
    std::cout << "Mismatching missing values: " << missingMismatched << "\n";
    std::cout << "Maximum absolute error: "
              << std::setprecision(12) << maximumError << "\n";
    std::cout << "Allowed packing tolerance: " << tolerance << "\n";

    bool passed = mismatched == 0;

    std::cout << "Numerical comparison: "
              << (passed ? "PASSED" : "FAILED") << "\n";

    return passed;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: decode_test <file.grib>\n";
        return 1;
    }

    std::cout << "Loading: " << argv[1] << "\n";

    auto dataset = griblite::GribLite::readFile(argv[1]);

    if (!dataset) {
        std::cerr << "Failed to decode GRIB file.\n";
        return 1;
    }

    std::cout << "\nSuccessfully decoded GRIB file!\n";

    const auto& fields = dataset->fields();

    std::cout << "Number of fields: " << fields.size() << "\n";

    // --------------------------------------------------
    // Test decoded values against Python reference data
    // --------------------------------------------------

    bool numericalTestPassed = true;

    if (argc >= 3) {
        try {
            auto reference = loadReferenceValues(argv[2]);

            if (fields.size() != 1) {
                std::cout << "\nNumerical test currently expects "
                        << "a file containing exactly one field.\n";

                numericalTestPassed = false;
            } else {
                numericalTestPassed = compareValues(
                    fields.front(),
                    reference
                );
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Reference comparison error: "
                    << e.what() << "\n";

            numericalTestPassed = false;
        }
    } else {
        std::cout << "\nNumerical comparison skipped.\n"
                << "Supply a reference file as the second argument.\n";
    }

    // --------------------------------------------------
    // Display information about every decoded field
    // --------------------------------------------------

    for (size_t i = 0; i < fields.size(); ++i) {
        const auto& field = fields[i];

        std::cout << "\n========================================\n";
        std::cout << "              FIELD " << i + 1 << "\n";
        std::cout << "========================================\n";

        std::cout << "Edition: " << field.edition << "\n";
        std::cout << "Parameter: " << field.parameter << "\n";
        std::cout << "Center: " << field.center << "\n";
        std::cout << "Discipline: " << field.discipline << "\n";

        std::cout << "Level type: " << field.levelType << "\n";
        std::cout << "Level value: " << field.levelValue << "\n";

        std::cout << "Reference time: " << field.refTime << "\n";
        std::cout << "Forecast hours: " << field.forecastHours << "\n";

        std::cout << "\n--- Grid Information ---\n";

        std::cout << "Grid dimensions: "
                  << field.grid.nx << " x "
                  << field.grid.ny << "\n";

        std::cout << "Grid supported: "
                  << (field.grid.supported ? "Yes" : "No") << "\n";

        std::cout << "\n--- Packing Information ---\n";

        std::cout << "Reference value: "
                  << field.referenceValue << "\n";

        std::cout << "Binary scale: "
                  << field.binaryScale << "\n";

        std::cout << "Decimal scale: "
                  << field.decimalScale << "\n";

        std::cout << "Bits per value: "
                  << field.bitsPerValue << "\n";

        std::cout << "Data template: "
                  << field.dataTemplate << "\n";

        std::cout << "Has bitmap: "
                  << (field.hasBitmap() ? "Yes" : "No") << "\n";

        std::cout << "\n--- Decoded Data ---\n";

        std::cout << "Decoded values: "
                  << field.values.size() << "\n";

        // Calculate statistics using only finite values.
        size_t finiteCount = 0;

        float minimum = std::numeric_limits<float>::max();
        float maximum = std::numeric_limits<float>::lowest();

        double sum = 0.0;

        for (float value : field.values) {
            if (!std::isfinite(value)) {
                continue;
            }

            finiteCount++;

            minimum = std::min(minimum, value);
            maximum = std::max(maximum, value);

            sum += value;
        }

        std::cout << "Finite values: " << finiteCount << "\n";
        std::cout << "Missing/non-finite values: "
                  << field.values.size() - finiteCount << "\n";

        if (finiteCount > 0) {
            double average = sum / finiteCount;

            std::cout << std::setprecision(10);

            std::cout << "Minimum: " << minimum << "\n";
            std::cout << "Maximum: " << maximum << "\n";
            std::cout << "Average: " << average << "\n";
        } else {
            std::cout << "No finite values available.\n";
        }

        std::cout << "\nField valid: "
                  << (field.isValid() ? "Yes" : "No") << "\n";

        // Display a small sample of decoded values.
        std::cout << "\nFirst 10 values:\n";

        size_t sampleCount = std::min(
            size_t(10),
            field.values.size()
        );

        for (size_t j = 0; j < sampleCount; ++j) {
            std::cout << "  [" << j << "] "
                      << field.values[j] << "\n";
        }
    }

    // --------------------------------------------------
    // Test the public findField() API
    // --------------------------------------------------

    std::cout << "\n========================================\n";
    std::cout << "             FIELD LOOKUP TEST\n";
    std::cout << "========================================\n";

    if (!fields.empty()) {
        const auto& firstField = fields.front();

        const griblite::GribField* foundField =
            dataset->findField(
                firstField.parameter,
                firstField.levelType,
                firstField.levelValue,
                firstField.forecastHours
            );

        if (foundField != nullptr) {
            std::cout << "Field lookup: SUCCESS\n";
            std::cout << "Found parameter: "
                      << foundField->parameter << "\n";
        } else {
            std::cout << "Field lookup: FAILED\n";
        }
    } else {
        std::cout << "No fields available to test lookup.\n";
    }

    std::cout << "\n========================================\n";
    std::cout << "             TEST COMPLETE\n";
    std::cout << "========================================\n";

    return numericalTestPassed ? 0 : 1;
}
