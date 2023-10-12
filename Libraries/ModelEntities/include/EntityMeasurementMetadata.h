#pragma once
#include "EntityWithDynamicFields.h"


class __declspec(dllexport) EntityMeasurementMetadata : public EntityWithDynamicFields
{
public:
	EntityMeasurementMetadata(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	std::string getClassName() override { return "EntityMeasurementMetadata"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	virtual void addVisualizationNodes() override;

	std::vector<std::string> getAllParameterDocumentNames();
	std::vector<std::string> getAllQuantityDocumentNames();
	const std::string getParameterDocumentName() const { return _parameterDocument; }
	const std::string getQuantityDocumentName() const { return _quantityDocument; }

	void InsertToParameterField(std::string fieldName, std::list<ot::Variable> values, std::string documentName = "");
	void InsertToQuantityField(std::string fieldName, std::list<ot::Variable> values, std::string documentName = "");
private:
	const std::string _parameterDocument = "Parameter";
	const std::string _quantityDocument = "Quantity";
};
