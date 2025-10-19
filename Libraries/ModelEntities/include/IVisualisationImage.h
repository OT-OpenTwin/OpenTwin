#pragma once

#include "OTGui/GuiTypes.h"

// std header
#include <vector>
#include <string>

class __declspec(dllexport) IVisualisationImage {
public:
	virtual ~IVisualisationImage() {};

	virtual const std::vector<char>& getImage() = 0;
	virtual ot::ImageFileFormat getImageFormat() const = 0;
	
	virtual bool visualiseImage() = 0;
};