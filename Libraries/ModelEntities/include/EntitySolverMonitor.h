#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"

class __declspec(dllexport) EntitySolverMonitor : public EntityContainer
{
public:

	EntitySolverMonitor(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntitySolverMonitor();

	virtual void addVisualizationNodes(void) override;

	virtual std::string getClassName(void) override { return "EntitySolverMonitor"; };

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;
	
	bool updatePropertyVisibilities(void);

	void createProperties();

	static const std::string GetPropertyNameDomain(void);
	static const std::string GetPropertyNameDoF(void);
	static const std::string GetPropertyNameFieldComp(void);
	static const std::string GetPropertyNameQuantity(void);
	static const std::string GetPropertyNameMonitorFrequency(void);
	static const std::string GetPropertyNameVolume(void);
	static const std::string GetPropertyValueVolumeFull(void);
	static const std::string GetPropertyValueVolumePoint(void);
	static const std::string GetPropertyNameXCoord(void);
	static const std::string GetPropertyNameYCoord(void);
	static const std::string GetPropertyNameZCoord(void);
	static const std::string GetPropertyNameObservedTimeStep(void);

	static const std::string GetPropertyValueFieldCompElectr(void);
	static const std::string GetPropertyValueFieldCompMagnetic(void);
	static const std::string GetPropertyValueDomainFrequency(void);
	static const std::string GetPropertyValueDomainTime(void);
	static const std::string GetPropertyValueDoFNode(void);
	static const std::string GetPropertyValueDoFEdge(void);
	static const std::string GetPropertyValueQuantityVector(void);
	static const std::string GetPropertyValueQuantityComponentX(void);
	static const std::string GetPropertyValueQuantityComponentY(void);
	static const std::string GetPropertyValueQuantityComponentZ(void);
	static const std::string GetPropertyNameObservFreq(void);

protected:
	virtual int getSchemaVersion(void) override { return 1; };

private:

};