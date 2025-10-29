// @otlicense

#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <list>
#include <vtkAlgorithmOutput.h>
#include <vtkDataSetMapper.h>

#include "vtkRectilinearGrid.h"
#include "VtkDriver.h"
#include "EntityResultBase.h"
#include "PropertyBundleDataHandlePlane.h"
#include "PropertyBundleDataHandleScaling.h"
#include "PropertyBundleDataHandleVis2D3D.h"
#include "DataSourceResult3D.h"

namespace osg {
	class Node;
}

class VtkDriverCartesianVectorfield : public VtkDriver {
public:
	VtkDriverCartesianVectorfield();
	virtual ~VtkDriverCartesianVectorfield();

	virtual void setProperties(EntityVis2D3D *visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) override;
	
private:
	PropertyBundleDataHandlePlane * planeData = nullptr;
	PropertyBundleDataHandleScaling * scalingData = nullptr;
	PropertyBundleDataHandleVis2D3D * vis2D3DData = nullptr;
	double * scalarRange = nullptr;

	vtkAlgorithmOutput* ApplyCutplane(DataSourceResult3D *source, osg::Node *parent);
	void AssembleNode (vtkAlgorithmOutput * input, osg::Node *parent);
	vtkAlgorithmOutput* AddNodeVectors(vtkAlgorithmOutput* input);
	vtkAlgorithmOutput* SetScalarValues(vtkAlgorithmOutput* input);
	void SetColouring(vtkPolyDataMapper* mapper);

	void CheckForModelUpdates();
	void DeletePropertyData(void);
};