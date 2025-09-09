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
	EntityFile(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityFile() = default;
	virtual bool getEntityBox(double &_xmin, double &_xmax, double &_ymin, double &_ymax, double &_zmin, double &_zmax) override;
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual std::string getClassName(void) override { return "EntityFile"; };
	virtual bool updateFromProperties(void) override;
	virtual void addVisualizationNodes() override;

	void setFileProperties(const std::string& _path, const std::string& _fileName, const std::string& _fileType);
	
	//! @brief Does not reset the data entity. If a new data entity is assinged, this topology entity will still hold a reference to the old one. However, typically one would update the old data entity.
	void setData(ot::UID _dataID, ot::UID _dataVersion);
	//! @brief  Requires the ClassFactoryHandler to be set.
	std::shared_ptr<EntityBinaryData> getData();

	std::string getPath() const { return m_path; }
	std::string getFileName() const { return m_fileName; }
	std::string getFileType() const { return m_fileType; }

	void setFileFilter(const std::string& _filter) { m_fileFilter = _filter; };
	std::string getFileFilter() const { return m_fileFilter; };

protected:
	virtual void setSpecializedProperties() {};
	virtual void AddStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase *>& _entityMap) override;


	long long m_dataUID = -1;
	long long m_dataVersion = -1;
private:
	std::string m_path ="";
	std::string m_fileName ="";
	std::string m_fileType ="";
	std::string m_fileFilter;

	std::shared_ptr<EntityBinaryData> m_data = nullptr;

	void EnsureDataIsLoaded();
	void setProperties();
	virtual int getSchemaVersion() { return 1; }
};