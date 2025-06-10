#pragma once
#include "EntityBase.h"
#include "IVisualisationCurve.h"
#include "OTGui/QueryInformation.h"

class __declspec(dllexport) EntityResult1DCurve : public EntityBase, public IVisualisationCurve
{
public:

	EntityResult1DCurve(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms, ClassFactoryHandler* _factory, const std::string& _owner);
	EntityResult1DCurve(const EntityResult1DCurve& _other) = default;
	EntityResult1DCurve(EntityResult1DCurve&& _other) noexcept = default;
	~EntityResult1DCurve() = default;
	EntityResult1DCurve& operator=(const EntityResult1DCurve& _other) = default;
	EntityResult1DCurve& operator=(EntityResult1DCurve&& _other) = default;

	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual void addVisualizationNodes(void) override;
	virtual bool updateFromProperties(void) override;

	virtual std::string getClassName(void) { return "EntityResult1DCurve_New"; };

	void createProperties(void);

	// Inherited via IVisualisationCurve
	bool visualiseCurve() override { return true; };
	ot::Plot1DCurveCfg getCurve() override;
	void setCurve(const ot::Plot1DCurveCfg& _curve) override;

private:
	ot::QueryInformation m_queryInformation;
	
	virtual void AddStorageData(bsoncxx::builder::basic::document& storage);
	bsoncxx::builder::basic::document serialise(ot::QuantityContainerEntryDescription& _quantityContainerEntryDescription);
	ot::QuantityContainerEntryDescription deserialise(bsoncxx::v_noabi::document::view _subDocument);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
