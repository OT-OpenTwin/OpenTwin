#pragma once
#include <array>
#include <cmath>
#include <complex>
#include <fstream>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>

#include "FarfieldImportData.h"

#include "OTCore/Units/SIUnits.h"

FarfieldImportData readFarfieldDataset(std::ifstream& stream, const std::string& filePath, std::size_t fileIndex, double frequency)
{
    
    FarfieldImportData result;

    auto initializeParameter = [](
        MetadataParameter& parameter,
        const std::string& name,
        const std::string& unit)
        {
            parameter.parameterName = name;
            parameter.typeName = ot::TypeNames::getDoubleTypeName();
            parameter.unit = unit;
        };

    initializeParameter(result.frequency, "Frequency", "MHz");

    result.frequency.values.emplace_back(
        ot::Variable(frequency)
    );
   
    initializeParameter(
        result.theta,
        "Theta",
        ot::SIUnits::getAngleUnitDegrees()
    );

    initializeParameter(
        result.phi,
        "Phi",
        ot::SIUnits::getAngleUnitDegrees()
    );

    result.directivity =
        std::make_unique<QuantityDescriptionCurve>();

    result.thetaComponent =
        std::make_unique<QuantityDescriptionCurve>();

    result.phiComponent =
        std::make_unique<QuantityDescriptionCurve>();

    result.axialRatio =
        std::make_unique<QuantityDescriptionCurve>();

    result.directivity->setName("Directivity");
    result.thetaComponent->setName("ThetaComponent");
    result.phiComponent->setName("PhiComponent");
    result.axialRatio->setName("AxialRatio");

   
    for (QuantityDescriptionCurve* quantity : {
        result.thetaComponent.get(),
        result.phiComponent.get()
        })
    {
        quantity->defineQuantityAsComplex(
            ot::ComplexNumberFormat::Polar,
            ot::TypeNames::getDoubleTypeName(),
            "",
            ot::SIUnits::getAngleUnitDegrees()
        );
    }

    const auto makePolarValue = [](
        double magnitudeDBi,
        double phaseDegrees) -> ot::Variable
        {
            const double linearMagnitude =
                std::pow(10.0, magnitudeDBi / 20.0);

            // storing magnitude and phase, as provided
            return ot::Variable(
                std::complex<double>(linearMagnitude, phaseDegrees)
            );
        };

    // Referenzraster der ersten Datei.
    // Die übrigen Dateien müssen dieselben Winkel in derselben
    // Reihenfolge enthalten.
    std::vector<std::pair<double, double>> referenceGrid;

        std::string line;

        // CST-Spaltenüberschrift und Trennlinie.
        if (!std::getline(stream, line))
        {
            throw std::runtime_error(
                "Missing header: " + filePath
            );
        }

        const std::string header = line;

        if (header.find("Theta") == std::string::npos ||
            header.find("Phi") == std::string::npos ||
            header.find("Abs(Dir.)") == std::string::npos ||
            header.find("Phase(Theta)") == std::string::npos ||
            header.find("Phase(Phi") == std::string::npos ||
            header.find("Ax.Ratio") == std::string::npos)
        {
            throw std::runtime_error(
                "Unexpected CST farfield header: "
                + filePath
            );
        }

        if (!std::getline(stream, line) ||
            line.find_first_not_of(" \t\r-") != std::string::npos)
        {
            throw std::runtime_error(
                "Missing or invalid header separator: "
                + filePath
            );
        }

        std::size_t lineNumber = 2;
        std::size_t rowIndex = 0;

        while (std::getline(stream, line))
        {
            ++lineNumber;

            if (line.find_first_not_of(" \t\r\n") ==
                std::string::npos)
            {
                continue;
            }

            std::istringstream row(line);
            row.imbue(std::locale::classic());

            // theta, phi, total, thetaMag, thetaPhase,
            // phiMag, phiPhase, axialRatio
            std::array<double, 8> values{};

            bool valid = true;

            for (double& value : values)
            {
                if (!(row >> value) || !std::isfinite(value))
                {
                    valid = false;
                    break;
                }
            }

            std::string extraToken;

            if (!valid || (row >> extraToken))
            {
                throw std::runtime_error(
                    "Invalid data row in " + filePath
                    + " at line " + std::to_string(lineNumber)
                    + ": expected exactly 8 finite numeric values."
                );
            }

            const double thetaDegrees = values[0];
            const double phiDegrees = values[1];

            if (fileIndex == 0)
            {
                referenceGrid.emplace_back(
                    thetaDegrees,
                    phiDegrees
                );
            }
            else
            {
                constexpr double angleTolerance = 1.0e-9;

                if (rowIndex >= referenceGrid.size() ||
                    std::abs(
                        thetaDegrees - referenceGrid[rowIndex].first
                    ) > angleTolerance ||
                    std::abs(
                        phiDegrees - referenceGrid[rowIndex].second
                    ) > angleTolerance)
                {
                    throw std::runtime_error(
                        "Angular grid differs in "
                        + filePath
                        + " at line "
                        + std::to_string(lineNumber)
                    );
                }
            }

            result.theta.values.emplace_back(
                ot::Variable(thetaDegrees)
            );

            result.phi.values.emplace_back(
                ot::Variable(phiDegrees)
            );

            result.directivity->addDatapoint(
                ot::Variable(values[2])
            );

            result.thetaComponent->addDatapoint(
                makePolarValue(values[3], values[4])
            );

            result.phiComponent->addDatapoint(
                makePolarValue(values[5], values[6])
            );

            result.axialRatio->addDatapoint(
                ot::Variable(values[7])
            );

            ++rowIndex;
            ++result.numberOfDatapoints;
        }

        if (stream.bad())
        {
            throw std::runtime_error(
                "Error while reading: " + filePath
            );
        }

        if (rowIndex == 0)
        {
            throw std::runtime_error(
                "No farfield data found: " + filePath
            );
        }

        if (fileIndex > 0 &&
            rowIndex != referenceGrid.size())
        {
            throw std::runtime_error(
                "Different number of angular samples in: "
                + filePath
            );
        }

    return result;
}