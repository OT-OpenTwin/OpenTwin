// @otlicense
// File: FarfieldReader.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

#include <cmath>
#include <complex>
#include <cstddef>
#include <fstream>
#include <initializer_list>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Read the entire ASCII far-field table in one call.
// Columns: Frequency Theta Phi Etheta_Abs Etheta_Phase Ephi_Abs Ephi_Phase Eabs
// Theta, Phi and phases are in degrees. Decimal separator: dot.
// Frequency units and all row ordering/duplicates are preserved.
//
// All six output arguments must refer to distinct vectors.
// Existing output is replaced after the entire file has been read successfully.
// true: at least one data row; false: no data rows (all outputs become empty).
// Errors throw std::runtime_error and leave the caller's vectors unchanged.
inline bool readFarFieldTable(
    const std::string& filename,
    std::vector<double>& frequency,
    std::vector<double>& theta,
    std::vector<double>& phi,
    std::vector<std::complex<double>>& eTheta,
    std::vector<std::complex<double>>& ePhi,
    std::vector<double>& eAbs)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    // Build results locally so that failed reads never return partial output.
    std::vector<double> newFrequency, newTheta, newPhi, newEAbs;
    std::vector<std::complex<double>> newETheta, newEPhi;

    const double degreesToRadians = std::acos(-1.0) / 180.0;
    std::size_t lineNumber = 0;
    std::string line;

    const auto invalidRow = [&lineNumber](const std::string& reason)
        {
            throw std::runtime_error("Invalid far-field data in line " +
                std::to_string(lineNumber) + ": " + reason);
        };

    while (std::getline(file, line))
    {
        ++lineNumber;

        // Accept an optional UTF-8 BOM at the start of the file.
        if (lineNumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);

        // Also accepts NumPy-style '# Frequency ...' headers.
        const auto comment = line.find('#');
        if (comment != std::string::npos)
            line.resize(comment);

        std::istringstream input(line);
        input.imbue(std::locale::classic());
        input >> std::ws;
        if (input.eof())
            continue;

        // Optional, un-commented header before the first data row.
        if (newFrequency.empty() && input.peek() == 'F')
        {
            const char* columns[] = {
                "Frequency", "Theta", "Phi", "Etheta_Abs",
                "Etheta_Phase", "Ephi_Abs", "Ephi_Phase", "Eabs"
            };
            std::string token;
            for (const char* column : columns)
            {
                if (!(input >> token) || token != column)
                    invalidRow("unexpected column header");
            }
            if (input >> token)
                invalidRow("unexpected extra header column");
            continue;
        }

        double f = 0.0, t = 0.0, p = 0.0;
        double etAbs = 0.0, etPhase = 0.0;
        double epAbs = 0.0, epPhase = 0.0, totalAbs = 0.0;

        if (!(input >> f >> t >> p >> etAbs >> etPhase
            >> epAbs >> epPhase >> totalAbs))
            invalidRow("expected eight numeric columns");

        std::string extra;
        if (input >> extra)
            invalidRow("unexpected extra column");

        for (double value : {f, t, p, etAbs, etPhase, epAbs, epPhase, totalAbs})
        {
            if (!std::isfinite(value))
                invalidRow("non-finite value");
        }
        if (etAbs < 0.0 || epAbs < 0.0 || totalAbs < 0.0)
            invalidRow("negative magnitude");

        newFrequency.push_back(f);
        newTheta.push_back(t);
        newPhi.push_back(p);
        newETheta.push_back(std::polar(etAbs, etPhase * degreesToRadians));
        newEPhi.push_back(std::polar(epAbs, epPhase * degreesToRadians));
        newEAbs.push_back(totalAbs);
    }

    if (file.bad() || !file.eof())
        throw std::runtime_error("I/O error while reading far-field data.");

    // Swapping vectors transfers the results without copying their elements.
    frequency.swap(newFrequency);
    theta.swap(newTheta);
    phi.swap(newPhi);
    eTheta.swap(newETheta);
    ePhi.swap(newEPhi);
    eAbs.swap(newEAbs);

    return !frequency.empty();
}

