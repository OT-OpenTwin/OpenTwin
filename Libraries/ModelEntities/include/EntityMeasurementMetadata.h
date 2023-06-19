#pragma once
#include "EntityWithDynamicFields.h"


class __declspec(dllexport) EntityMeasurementMetadata : public EntityWithDynamicFields
{
public:
	EntityMeasurementMetadata(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, ClassFactory* factory, const std::string& owner);
	std::string getClassName() override { return "EntityMeasurementMetadata"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	virtual void addVisualizationNodes() override;

	std::vector<std::string> getAllParameterDocumentNames();
	std::vector<std::string> getAllQuantityDocumentNames();
	const std::string getParameterDocumentName() const { return _parameterDocument; }
	const std::string getQuantityDocumentName() const { return _quantityDocument; }


	template <class T>
	void InsertToParameterField(std::string fieldName, std::list<T> values, std::string documentName = "");
	template <class T>
	void InsertToQuantityField(std::string fieldName, std::list<T> values, std::string documentName = "");
private:
	const std::string _parameterDocument = "Parameter";
	const std::string _quantityDocument = "Quantity";
};

template<class T>
inline void EntityMeasurementMetadata::InsertToParameterField(std::string fieldName, std::list<T> values, std::string documentName)
{
	std::string fullDocumentPath = _parameterDocument;
	if (documentName != "")
	{
		fullDocumentPath += "/" + documentName;
	}
	InsertInField(fieldName, values, fullDocumentPath);
}

template<class T>
inline void EntityMeasurementMetadata::InsertToQuantityField(std::string fieldName, std::list<T> values, std::string documentName)
{
	std::string fullDocumentPath = _quantityDocument;
	if (documentName != "")
	{
		fullDocumentPath += "/" + documentName;
	}
	InsertInField(fieldName, values, fullDocumentPath);
}

