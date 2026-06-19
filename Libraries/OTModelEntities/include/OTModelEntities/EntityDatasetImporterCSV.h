//@otlicense
#pragma once

#include "OTModelEntities/EntityBase.h"
#include <map>

class __declspec(dllexport) EntityDatasetImporterCSV : public EntityBase
{
public:
	EntityDatasetImporterCSV() : EntityDatasetImporterCSV(0, nullptr, nullptr, nullptr) {};
	EntityDatasetImporterCSV(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);

	enum class MetadataSelectionFormat
	{
		None, SameName
	};

	enum class CSVSelectionFormat
	{
		All, Regex
	};

	enum class NamingFormat
	{
		TableName, Custom
	};

	void createProperties(const std::string& _rmdClassificationFolderName, ot::UID _rmdClassificationFolderUID);
	bool getEntityBox(double& _xmin, double& _xmax, double& _ymin, double& _ymax, double& _zmin, double& _zmax) override { return false; };
	entityType getEntityType(void) const override { return entityType::TOPOLOGY; };
	virtual void addVisualizationNodes(void) override;
	virtual bool updateFromProperties(void) override;

	static std::string className() { return "EntityDatasetImporterCSV"; };
	virtual std::string getClassName(void) const override { return EntityDatasetImporterCSV::className(); };

	CSVSelectionFormat getCSVSelectionFormat();
	std::string getCSVSelectionFormatString();
	std::string getCSVSelectionRegex();
	MetadataSelectionFormat getMetadataSelectionFormat();
	std::string getMetadataSelectionFormatString();
	NamingFormat getNamingFormat();
	std::string getNamingFormatString();
	std::string getNamingBase();
	bool interruptAtWarnings();
	std::string getSelectedSeriesClassification();

private:
	static std::map<std::string, MetadataSelectionFormat> m_metadataIdentificationsByName;
	static std::map<std::string, CSVSelectionFormat> m_consideredCSVByName;
	static std::map<std::string, NamingFormat> m_namingFormatByName;
};
