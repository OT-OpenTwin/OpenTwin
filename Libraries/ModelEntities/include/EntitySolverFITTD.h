#pragma once
#pragma warning(disable : 4251)

#include "EntitySolver.h"
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntitySolverFITTD : public EntitySolver
{
public:
	EntitySolverFITTD() : EntitySolverFITTD(0, nullptr, nullptr, nullptr, "") {};
	EntitySolverFITTD(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntitySolverFITTD();

	virtual std::string getClassName(void) override { return "EntitySolverFITTD"; } ;

	void createProperties(const std::string meshFolderName, ot::UID meshFolderID, const std::string meshName, ot::UID meshID);
	virtual bool updateFromProperties(void) override;

	static const std::string GetPropertyNameDebug();
	static const std::string GetPropertyNameMesh();
	static const std::string GetPropertyNameTimesteps();
	static const std::string GetPropertyNameDimension();
	static const std::string GetPropertyNameVolumeSelection();

	static const std::string GetPropertyValueDimension3D();
	static const std::string GetPropertyValueVolumeFull();
	

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

