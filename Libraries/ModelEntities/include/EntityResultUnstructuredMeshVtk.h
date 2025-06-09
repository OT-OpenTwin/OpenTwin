#pragma once
#pragma warning(disable : 4251)

#include <list>

#include "EntityBase.h"
#include "EntityResultBase.h"
#include "OldTreeIcon.h"
#include "EntityBinaryData.h"

class __declspec(dllexport) EntityResultUnstructuredMeshVtk : public EntityBase, public EntityResultBase
{
public:
	enum eQuantityType {SCALAR, VECTOR};

	EntityResultUnstructuredMeshVtk(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityResultUnstructuredMeshVtk();
	
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	
	virtual std::string getClassName(void) { return "EntityResultUnstructuredMeshVtk"; };
	
	virtual entityType getEntityType(void) const override { return DATA;};

	void getData(std::string& quantityName, eQuantityType &quantityType, std::vector<char>& data, ClassFactoryHandler* factory);

	// Please note that setting the data also transfers the ownership of the EntityBinaryData object. The object must not be deleted outside the EntityResultUnstructuredMesh.
	void setData(const std::string &quantityName, eQuantityType quantityType, EntityBinaryData*& data);

private:
	
	std::string _quantityName;
	eQuantityType _quantityType = SCALAR;

	EntityBinaryData* _vtkData = nullptr;

	long long _vtkDataID = -1;
	long long _vtkDataVersion = -1;

	void clearAllBinaryData(void);

	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};
