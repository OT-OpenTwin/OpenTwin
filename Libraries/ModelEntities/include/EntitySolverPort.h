// @otlicense

#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"

class __declspec(dllexport) EntitySolverPort : public EntityContainer
{
public:
	EntitySolverPort() : EntitySolverPort(0, nullptr, nullptr, nullptr, "") {};
	EntitySolverPort(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntitySolverPort();

	static std::string className() { return "EntitySolverPort"; }
	virtual std::string getClassName(void) const override { return EntitySolverPort::className(); }

	virtual void addVisualizationNodes(void) override;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;

	void createProperties(const std::string signalTypeFolderName, ot::UID signalTypeFolderID, std::string signalName, ot::UID signalUID);

	static const std::string GetPropertyNameSignal();
	static const std::string GetPropertyNameSelectionMethod();
	static const std::string GetPropertyNameXCoordinate();
	static const std::string GetPropertyNameYCoordinate();
	static const std::string GetPropertyNameZCoordinate();
	static const std::string GetPropertyNameExcitationAxis();
	static const std::string GetPropertyNameExcitationTarget();
	
	static const std::string GetValueSelectionMethodCentre();
	static const std::string GetValueSelectionMethodFree();
	
	static const std::string GetValueExcitationAxisX();
	static const std::string GetValueExcitationAxisY();
	static const std::string GetValueExcitationAxisZ();
	
	static const std::string GetValueExcitationTargetE();
	static const std::string GetValueExcitationTargetH();


protected:
	bool UpdateVisibillity();

	virtual int getSchemaVersion(void) override { return 1; };
};