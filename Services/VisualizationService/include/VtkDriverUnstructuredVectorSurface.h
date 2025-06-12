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
#include "PropertyBundleDataHandleVisUnstructuredVectorSurface.h"
#include "DataSourceUnstructuredMesh.h"

namespace osg {
	class Node;
}

class VtkDriverUnstructuredVectorSurface : public VtkDriver {
public:
	VtkDriverUnstructuredVectorSurface();
	virtual ~VtkDriverUnstructuredVectorSurface();

	virtual void setProperties(EntityVis2D3D *visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) override;
	
private:
	PropertyBundleDataHandleScaling * scalingData = nullptr;
	PropertyBundleDataHandleVisUnstructuredVectorSurface * visData = nullptr;
	double * scalarRange = nullptr;

	void AssembleNode(osg::Node* parent);

	void AddNodeVectors(vtkAlgorithmOutput* input, osg::Node* parent);
	vtkAlgorithmOutput* SetScalarValues(void);
	void SetColouring(vtkPolyDataMapper* mapper);
	vtkAlgorithmOutput* GetArrowSource(void);

	void CheckForModelUpdates();
	void DeletePropertyData(void);

	DataSourceUnstructuredMesh* dataSource;
	vtkAlgorithmOutput* dataConnection;

	std::list<vtkObject*> objectsToDelete;
};