#pragma once
#include "EntityContainer.h"
#include "IVisualisationPlot1D.h"
class __declspec(dllexport) EntityResult1DPlot_New : public EntityContainer, public IVisualisationPlot1D
{
public:
	EntityResult1DPlot_New(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, ClassFactoryHandler* _factory, const std::string& _owner);

	bool getEntityBox(double& _xmin, double& _xmax, double& _ymin, double& _ymax, double& _zmin, double& _zmax) override { return false; };
	entityType getEntityType(void) const override { return entityType::TOPOLOGY; };
	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };
	virtual void StoreToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	virtual std::string getClassName(void) override { return "EntityResult1DPlot_New"; };
	virtual bool updateFromProperties(void) override;
	bool updatePropertyVisibilities(void);
	
	void createProperties(void);

	// Inherited via IVisualisationPlot1D
	const ot::Plot1DCfg getPlot() override;
	void setPlot(const ot::Plot1DCfg& _config) override;

	bool visualisePlot() override;

private:
	virtual void AddStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) override;

};
