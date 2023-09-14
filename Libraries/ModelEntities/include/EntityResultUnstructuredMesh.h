#pragma once
#pragma warning(disable : 4251)

#include <list>

#include "EntityBase.h"
#include "Types.h"
#include "EntityBinaryData.h"

class __declspec(dllexport) EntityResultUnstructuredMesh : public EntityBase
{
public:
	EntityResultUnstructuredMesh(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual ~EntityResultUnstructuredMesh();
	
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	
	virtual std::string getClassName(void) { return "EntityResultUnstructuredMesh"; };
	
	virtual entityType getEntityType(void) override { return DATA;};

	void GetPointCoordData(size_t& numberPoints, double *&x, double*& y, double*& z, ClassFactory* factory);
	void GetCellData(size_t& numberCells, size_t& sizeCellData, int*& cells, ClassFactory* factory);

	// Please note that setting the data also transfers the ownership of the EntityBinaryData objects. The objects must not be deleted outside the EntityResultUnstructuredMesh.
	void setMeshData(size_t numberPoints, size_t numberCells, size_t sizeCellData, EntityBinaryData*& xcoord, EntityBinaryData*& ycoord, EntityBinaryData*& zcoord, EntityBinaryData*& cellData);

private:
	
	long long _numberPoints = 0;
	long long _numberCells  = 0;
	long long _sizeCellData = 0;

	EntityBinaryData* _xcoord = nullptr;
	EntityBinaryData* _ycoord = nullptr;
	EntityBinaryData* _zcoord = nullptr;

	EntityBinaryData* _cellData = nullptr;

	long long _xCoordDataID = -1;
	long long _xCoordDataVersion = -1;
	long long _yCoordDataID = -1;
	long long _yCoordDataVersion = -1;
	long long _zCoordDataID = -1;
	long long _zCoordDataVersion = -1;

	long long _cellDataID = -1;
	long long _cellDataVersion = -1;

	void clearAllBinaryData(void);
	void updateIDFromObjects(void);

	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};