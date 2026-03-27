// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/EntityBase.h"

namespace ot {

	class OT_MODELENTITIES_API_EXPORT EntityDatasetInfo : public EntityBase
	{
		OT_DECL_NOCOPY(EntityDatasetInfo)
		OT_DECL_NOMOVE(EntityDatasetInfo)
	public:
		EntityDatasetInfo() : EntityDatasetInfo(0, nullptr, nullptr, nullptr) {};
		EntityDatasetInfo(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms);
		virtual ~EntityDatasetInfo() = default;

		virtual entityType getEntityType() const override { return TOPOLOGY; };
		virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

		virtual void addVisualizationNodes() override;
		virtual bool updateFromProperties() override;

		static std::string className() { return "EntityDatasetInfo"; }
		virtual std::string getClassName() const override { return EntityDatasetInfo::className(); };

		void createProperties(const std::string& _initialProjectName);

		void setProjectName(const std::string& _projectName);
		std::string getProjectName() const;

	private:

	};
}