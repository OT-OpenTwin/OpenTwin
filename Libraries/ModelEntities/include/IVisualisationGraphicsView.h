#pragma once
#include "OTGui/GraphicsPackage.h"
class IVisualisationGraphicsView
{
public:

	virtual ot::GraphicsNewEditorPackage* getGraphicsEditorPackage() = 0;
	virtual bool visualiseGraphicsView() = 0;
};
