#pragma once
#pragma warning(disable : 4251)

#include <list>

#include "EntityBase.h"
#include "EntityResultBase.h"
#include "OldTreeIcon.h"
#include "EntityCompressedVector.h"
#include "EntityMeshCartesian.h"
#include "EntityMeshCartesianData.h"
#include "OldTreeIcon.h"

class __declspec(dllexport) EntityResult3DData : public EntityBase , public EntityResultBase
{
public:
	EntityResult3DData() : EntityResult3DData(0, nullptr, nullptr, nullptr, "") {};
	EntityResult3DData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityResult3DData();
	
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	
	virtual std::string getClassName(void) override { return "EntityResult3DData"; };
	
	virtual entityType getEntityType(void) const override { return DATA;};

	/**
	 * @brief  .
	 * 
	 * @throws invalid_argument if not all UIDs are set
	 */
	virtual void storeToDataBase(void) override;
	void LoadAllData(void);
	
	void SetVectorData(EntityCompressedVector * xComponent, EntityCompressedVector *yComponent, EntityCompressedVector *zComponent);
	void SetVectorDataUIDs(ot::UID xComponentID, ot::UID xComponentVersion, ot::UID yComponentID, ot::UID yComponentVersion, ot::UID zComponentID, ot::UID zComponentVersion);

	void setMeshData(ot::UID meshID, ot::UID meshDataVersion);

	/**
	 * @brief  .
	 * 
	 * @param size
	 * @throws invalid_argument if ot::UID of entity is not set. 
	 * @return 
	 */
	const double* GetXComponentData(size_t & size);

	/**
	 * @brief  .
	 * 
	 * @param size
	 * @throws invalid_argument if ot::UID of entity is not set. 
	 * @return 
	 */
	const double* GetYComponentData(size_t & size);

	/**
	 * @brief  .
	 * 
	 * @param size
	 * @throws invalid_argument if ot::UID of entity is not set. 
	 * @return 
	 */
	const double* GetZComponentData(size_t & size);

	/**
	 * @brief  .
	 * 
	 * @param size
	 * @throws invalid_argument if ot::UID of entity is not set. 
	 * @return 
	 */
	const double* GetXComponentDataAndReleaseEntity(size_t & size);

	/**
	 * @brief  .
	 * 
	 * @param size
	 * @throws invalid_argument if ot::UID of entity is not set. 
	 * @return 
	 */
	const double* GetYComponentDataAndReleaseEntity(size_t & size);

	/**
	 * @brief  .
	 * 
	 * @param size
	 * @throws invalid_argument if ot::UID of entity is not set. 
	 * @return 
	 */
	const double* GetZComponentDataAndReleaseEntity(size_t & size);

	/**
	 * @brief  .
	 * 
	 * @throws invalid_argument if ot::UID or version of entity is not set. 
	 * @throws invalid_argument if Meshdata cannot be loaded.
	 * @return 
	 */
	EntityMeshCartesianData * GetMeshData(void);

	void clearData(void);
	void clearXComponentData(void);
	void clearYComponentData(void);
	void clearZComponentData(void);

private:
	
	EntityCompressedVector * _xComponentData = nullptr;
	EntityCompressedVector * _yComponentData = nullptr;
	EntityCompressedVector * _zComponentData = nullptr;

	EntityMeshCartesianData * _meshData = nullptr;

	long long _xComponentDataID = -1;
	long long _xComponentDataVersion = -1;
	long long _yComponentDataID = -1;
	long long _yComponentDataVersion = -1;
	long long _zComponentDataID = -1;
	long long _zComponentDataVersion = -1;

	long long _meshDataID = -1;
	long long _meshDataVersion = -1;

	void DeleteAllCompressedVectors();

	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};