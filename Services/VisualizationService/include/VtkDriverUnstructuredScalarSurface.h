// @otlicense

#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <list>
#include <vtkAlgorithmOutput.h>
#include <vtkDataSetMapper.h>

#include "VtkDriver.h"
#include "EntityResultBase.h"
#include "PropertyBundleDataHandleScaling.h"
#include "PropertyBundleDataHandleVisUnstructuredScalarSurface.h"
#include "DataSourceUnstructuredMesh.h"

namespace osg {
	class Node;
}

class VtkDriverUnstructuredScalarSurface : public VtkDriver {
public:
	VtkDriverUnstructuredScalarSurface();
	virtual ~VtkDriverUnstructuredScalarSurface();

	virtual void setProperties(EntityVis2D3D* visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem* dataItem) override;

private:
	PropertyBundleDataHandleScaling* scalingData = nullptr;
	PropertyBundleDataHandleVisUnstructuredScalarSurface* visData = nullptr;
	double* scalarRange = nullptr;

	void AssembleNode(osg::Node* parent);

	void AddNodeContour(osg::Node* parent);
	void AddNodePoints(osg::Node* parent);
	void SetColouring(vtkPolyDataMapper* mapper);
	vtkAlgorithmOutput* GetPointSource(void);

	void CheckForModelUpdates();
	void DeletePropertyData(void);

	DataSourceUnstructuredMesh* dataSource;
	vtkAlgorithmOutput* dataConnection;

	std::list<vtkObject*> objectsToDelete;
};