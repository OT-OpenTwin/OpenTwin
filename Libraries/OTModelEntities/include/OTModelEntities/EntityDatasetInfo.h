// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/Visualization/IVisualisationTable.h"
#include "OTModelEntities/IEventHandler.h"
#include "OTModelEntities/Properties/Bundle/PropertyBundleEventHandling.h"
#include "OTModelEntities/Lms/EntityBlockLibraryInterface.h"
#include "OTModelEntities/Lms/LibraryElement.h"

namespace ot {

	class OT_MODELENTITIES_API_EXPORT EntityDatasetInfo : public EntityBase, public IVisualisationTable, public IEventHandler, public EntityBlockLibraryInterface
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

		void createProperties(const std::string& _initialProjectName, ot::UID _scriptFolder, ot::UID _manifestFolder);

		virtual ot::GenericDataStructMatrix getTable() override;
		virtual void setTable(const ot::GenericDataStructMatrix& _table) override;
		virtual ot::TableCfg getTableConfig(bool _includeData) override;
		virtual char getDecimalDelimiter() override;
		virtual bool visualiseTable() override;
		virtual ot::TableCfg::TableHeaderMode getHeaderMode() override;

		virtual void setActiveFilters(const std::list<ValueComparisonDescription>& _filters) override;
		virtual std::list<ValueComparisonDescription> getActiveFilters() const override { return m_activeFilters; };

		void setProjectName(const std::string& _projectName);
		std::string getProjectName() const;

		std::string getScriptName() override;
		std::string getEnvironmentName() override;
		std::optional<std::string>getEventHandlingFunction(PythonEventType _type, std::map<ot::UID, EntityBase*>& _entityMap) override;

		// Special library interface function
		virtual std::list<ot::LibraryElement> libraryElementWasSet(const ot::LibraryElement& _libraryElement, EntityBase* _entity, ot::NewModelStateInfo& _newStateInfo) override;

		virtual void nonValuePropertyValueSelected(const EntityPropertiesBase* _property) override;

	protected:

		virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
		virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

	private:
		PropertyBundleEventHandling m_propertyBundleEventHandling;
		std::list<ValueComparisonDescription> m_activeFilters;
	};
}