#pragma once
#include "EntityWithDynamicFields.h"


class __declspec(dllexport) EntityMetadataSeries : public EntityWithDynamicFields
{
public:
	EntityMetadataSeries() : EntityMetadataSeries(0, nullptr, nullptr, nullptr, "") {};
	EntityMetadataSeries(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, const std::string& owner);
	std::string getClassName() const override { return "EntityMetadataSeries"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	virtual void addVisualizationNodes() override;

	std::vector<std::string> getAllParameterDocumentNames();
	std::vector<std::string> getAllQuantityDocumentNames();
	const std::string getParameterDocumentName() const { return _parameterDocument; }
	const std::string getQuantityDocumentName() const { return _quantityDocument; }

	void InsertToParameterField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName = "");
	void InsertToQuantityField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName = "");
private:
	const std::string _parameterDocument = "Parameter";
	const std::string _quantityDocument = "Quantity";
};
