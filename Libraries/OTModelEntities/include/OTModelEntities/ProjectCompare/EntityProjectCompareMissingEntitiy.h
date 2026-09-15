// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/EntityBase.h"

namespace ot
{

	class OT_MODELENTITIES_API_EXPORT EntityProjectCompareMissingEntitiy : public EntityBase
	{
	public:
		EntityProjectCompareMissingEntitiy() : EntityProjectCompareMissingEntitiy(ot::invalidUID, nullptr, nullptr, nullptr) {};
		EntityProjectCompareMissingEntitiy(ot::UID _id, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms);
		virtual ~EntityProjectCompareMissingEntitiy();

		static std::string className() { return "EntityProjectCompareMissingEntitiy"; };
		virtual std::string getClassName() const override { return EntityProjectCompareMissingEntitiy::className(); };

		virtual entityType getEntityType() const override { return TOPOLOGY; };

		void createProperties();

		virtual bool updateFromProperties() override;

		virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Property setter / getter

		void setCompareEntityType(const std::string& _entityType);
		std::string getCompareEntityType() const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected base class methods

	protected:
		virtual int getSchemaVersion() override { return 1; };

		virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
		virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

	};

}