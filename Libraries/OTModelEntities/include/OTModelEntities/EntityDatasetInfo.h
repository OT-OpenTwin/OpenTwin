// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/Visualization/IVisualisationTable.h"

namespace ot {

	class OT_MODELENTITIES_API_EXPORT EntityDatasetInfo : public EntityBase, public IVisualisationTable
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

		virtual ot::GenericDataStructMatrix getTable() override;
		virtual void setTable(const ot::GenericDataStructMatrix& _table) override;
		virtual ot::TableCfg getTableConfig(bool _includeData) override;
		virtual char getDecimalDelimiter() override;
		virtual bool visualiseTable() override;
		virtual ot::TableCfg::TableHeaderMode getHeaderMode() override;

		void setProjectName(const std::string& _projectName);
		std::string getProjectName() const;

	private:

	};
}