#pragma once
#include "EntityBase.h"
class __declspec(dllexport) EntityDatabaseIndexManipulator : public EntityBase
{
	public:
		EntityDatabaseIndexManipulator(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner)
			: EntityBase(_ID, _parent, _obs, _ms, _owner) {};
		EntityDatabaseIndexManipulator() : EntityDatabaseIndexManipulator(0, nullptr, nullptr, nullptr, "") {};
		virtual ~EntityDatabaseIndexManipulator() =default;
		void createProperties(const std::string& _projectName);

		static std::string className() { return "EntityDatabaseIndexManipulator"; };
		void removeAllIndexes();
		void createIndexes(const std::list<std::string>& _newIndexNames);
		const std::string getSelectedProjectName();
		bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; }

		entityType getEntityType(void) const override { return entityType::TOPOLOGY; }
private:
};
