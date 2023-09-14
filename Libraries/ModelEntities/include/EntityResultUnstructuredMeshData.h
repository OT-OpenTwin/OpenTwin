#pragma once
#pragma warning(disable : 4251)

#include <list>

#include "EntityBase.h"
#include "EntityResultBase.h"
#include "Types.h"
#include "EntityBinaryData.h"

class __declspec(dllexport) EntityResultUnstructuredMeshData : public EntityBase, public EntityResultBase
{
public:
	EntityResultUnstructuredMeshData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual ~EntityResultUnstructuredMeshData();
	
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	
	virtual std::string getClassName(void) { return "EntityResultUnstructuredMeshData"; };
	
	virtual entityType getEntityType(void) override { return DATA;};

	long long getNumberOfPoints(void) { return _numberPoints; };
	long long getNumberOfCells(void) { return _numberCells; };

	void getData(size_t& lenPointScalar, float*& pointScalar, size_t& lenPointVector, float*& pointVector, size_t& lenCellScalar, float*& cellScalar, size_t& lenCellVector, float*& cellVector, ClassFactory* factory);

	// Please note that setting the data also transfers the ownership of the EntityBinaryData objects. The objects must not be deleted outside the EntityResultUnstructuredMesh.
	void setData(long long numberPoints, long long numberCells, EntityBinaryData*& pointScalar, EntityBinaryData*& pointVector, EntityBinaryData*& cellScalar, EntityBinaryData*& cellVector);

private:
	
	long long _numberPoints = 0;
	long long _numberCells = 0;

	EntityBinaryData* _pointScalar = nullptr;
	EntityBinaryData* _pointVector = nullptr;
	EntityBinaryData* _cellScalar = nullptr;
	EntityBinaryData* _cellVector = nullptr;

	long long _pointScalarID = -1;
	long long _pointScalarVersion = -1;
	long long _pointVectorID = -1;
	long long _pointVectorVersion = -1;
	long long _cellScalarID = -1;
	long long _cellScalarVersion = -1;
	long long _cellVectorID = -1;
	long long _cellVectorVersion = -1;

	void clearAllBinaryData(void);
	void updateIDFromObjects(void);
	void readData(EntityBinaryData* data, size_t& lenValues, float*& values);

	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};
