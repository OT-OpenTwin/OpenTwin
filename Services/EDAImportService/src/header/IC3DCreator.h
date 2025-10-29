// @otlicense

#ifndef IC3DCREATOR_H
#define IC3DCREATOR_H

#include "IC.h"
#include "Viewer.h"
#include "ShapeUtilities.h"
#include "STEPWriter.h"
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>

class IC3DCreator
{
public:
	void createIC(IC& ic, STEPWriter& stepWriter, Viewer& viewer);
};

#endif // IC3DCREATOR_H