#pragma once
#pragma warning(disable : 4251)

#include "EntitySolver.h"
#include "OldTreeIcon.h"

#include <list>
#include <string>

class __declspec(dllexport) EntitySolverPyrit : public EntitySolver
{
public:
	EntitySolverPyrit() : EntitySolverPyrit(0, nullptr, nullptr, nullptr, "") {};
	EntitySolverPyrit(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntitySolverPyrit();

	virtual std::string getClassName(void) const override { return "EntitySolverPyrit"; } ;

	void createProperties(std::string& _meshFolderName, ot::UID& _meshFolderID, std::string& _meshName, ot::UID& _meshID,
					      std::string& _scriptFolderName, ot::UID& _scriptFolderID, std::string& _scriptName, ot::UID& _scriptID);

	virtual bool updateFromProperties(void) override;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

