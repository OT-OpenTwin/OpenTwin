#pragma once
#include <memory>
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurve.h"
#include "OTCore/MetadataHandle/MetadataParameter.h"
struct FarfieldImportData
{
    // Zeilenweise Koordinaten:
    // frequency.values[i], theta.values[i], phi.values[i]
    // gehören zu Datenpunkt i jeder Quantity.
    MetadataParameter frequency;
    MetadataParameter theta;
    MetadataParameter phi;

    std::unique_ptr<QuantityDescriptionCurve> directivity;
    std::unique_ptr<QuantityDescriptionCurve> thetaComponent;
    std::unique_ptr<QuantityDescriptionCurve> phiComponent;
    std::unique_ptr<QuantityDescriptionCurve> axialRatio;

    std::size_t numberOfDatapoints = 0;
};