#pragma once
#pragma once
#include "EntityBase.h"
#include "EntityBinaryData.h"

class __declspec(dllexport) EntityParameterizedDataSource : public EntityBase
{
	public:
	EntityParameterizedDataSource(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	~EntityParameterizedDataSource();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual std::string getClassName(void) override { return "EntityParameterizedDataSource"; };
	virtual void StoreToDataBase() override;
	virtual bool updateFromProperties(void) override;
	virtual void addVisualizationNodes() override;

	void setFileProperties(std::string path, std::string fileName, std::string fileType);
	void setData(ot::UID dataID, ot::UID dataVersion);
	void loadData();
	EntityBinaryData * getData() { return _data; };
	std::string getPath() const { return _path; }
	std::string getFileName() const { return _fileName; }
	std::string getFileType() const { return _fileType; }

protected:
	virtual void setSpecializedProperties() {};
	virtual void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

private:
	std::string _path ="";
	std::string _fileName ="";
	std::string _fileType ="";
	long long _dataUID;
	long long _dataVersion;

	EntityBinaryData * _data = nullptr;
	
	void clearData();
	void setProperties();
	virtual int getSchemaVersion() { return 1; };
};