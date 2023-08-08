/*****************************************************************//**
 * \file   EntityFile.h
 * \brief  Entity that holds a binary representation of a file. Any type of file should be supported
 * 
 * \author Wagner
 * \date   August 2023
 *********************************************************************/
#pragma once
#include "EntityBase.h"
#include "EntityBinaryData.h"
#include <memory>

class __declspec(dllexport) EntityFile: public EntityBase
{
public:
	EntityFile(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual std::string getClassName(void) override { return "EntityFile"; };
	virtual bool updateFromProperties(void) override;
	virtual void addVisualizationNodes() override;

	void setFileProperties(std::string path, std::string fileName, std::string fileType);
	void setData(ot::UID dataID, ot::UID dataVersion);
	std::shared_ptr<EntityBinaryData> getData();
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
	long long _dataUID = -1;
	long long _dataVersion = -1;

	void EnsureDataIsLoaded();
	std::shared_ptr<EntityBinaryData> _data = nullptr;
	
	void setProperties();
	virtual int getSchemaVersion() { return 1; }
};