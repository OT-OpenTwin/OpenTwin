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
#include "PropertyBundleDataHandleVisUnstructuredScalar.h"
#include "DataSourceUnstructuredMesh.h"

namespace osg {
	class Node;
}

class VtkDriverUnstructuredScalarVolume : public VtkDriver {
public:
	VtkDriverUnstructuredScalarVolume();
	virtual ~VtkDriverUnstructuredScalarVolume();

	virtual void setProperties(EntityVis2D3D* visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem* dataItem) override;

private:
	PropertyBundleDataHandlePlane* planeData = nullptr;
	PropertyBundleDataHandleScaling* scalingData = nullptr;
	PropertyBundleDataHandleVisUnstructuredScalar* visData = nullptr;
	double* scalarRange = nullptr;

	vtkAlgorithmOutput* ApplyCutplane(osg::Node* parent);
	void Assemble2DNode(osg::Node* parent);
	void Assemble3DNode(osg::Node* parent);
	void SetPlaneProperties(vtkPlane* plane);

	void AddNodePoints(osg::Node* parent);
	void AddIsosurfaces(osg::Node* parent);
	void SetColouring(vtkPolyDataMapper* mapper);
	vtkAlgorithmOutput* GetPointSource(void);

	void CheckForModelUpdates();
	void DeletePropertyData(void);

	DataSourceUnstructuredMesh* dataSource;
	vtkAlgorithmOutput* dataConnection;

	std::list<vtkObject*> objectsToDelete;
};