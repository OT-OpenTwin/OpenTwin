// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/EntityBase.h"

namespace ot
{

	class OT_MODELENTITIES_API_EXPORT EntityMDFFile : public EntityBase
	{
		OT_DECL_NOCOPY(EntityMDFFile)
		OT_DECL_NOMOVE(EntityMDFFile)
	public:
		EntityMDFFile() : EntityMDFFile(0, nullptr, nullptr, nullptr) {};
		explicit EntityMDFFile(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);
		virtual ~EntityMDFFile() = default;

		virtual entityType getEntityType() const override { return TOPOLOGY; };
		static std::string className() { return "EntityMDFFile"; };
		virtual std::string getClassName() const override { return EntityMDFFile::className(); };
		virtual bool getEntityBox(double&, double&, double&, double&, double&, double&) override { return false; };

		virtual bool updateFromProperties() override;

		virtual std::string serialiseAsJSON() override;
		virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept;

	protected:
		virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
		virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;
	};

}