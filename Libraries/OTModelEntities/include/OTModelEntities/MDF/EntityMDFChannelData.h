// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/EntityBase.h"

namespace ot
{

	class OT_MODELENTITIES_API_EXPORT EntityMDFChannelData : public EntityBase
	{
		OT_DECL_NOCOPY(EntityMDFChannelData)
		OT_DECL_NOMOVE(EntityMDFChannelData)
	public:
		EntityMDFChannelData() : EntityMDFChannelData(0, nullptr, nullptr, nullptr) {};
		explicit EntityMDFChannelData(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);
		virtual ~EntityMDFChannelData() = default;

		virtual entityType getEntityType() const override { return DATA; };
		static std::string className() { return "EntityMDFChannelData"; };
		virtual std::string getClassName() const override { return EntityMDFChannelData::className(); };
		virtual bool getEntityBox(double&, double&, double&, double&, double&, double&) override { return false; };

		virtual std::string serialiseAsJSON() override;
		virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept;

		void setSamples(const std::vector<double>& _samples) { m_samples = _samples; setModified(); };
		void setSamples(std::vector<double>&& _samples) { m_samples = std::move(_samples); setModified(); };
		const std::vector<double>& getSamples() const { return m_samples; };

	protected:
		virtual int getSchemaVersion(void) override { return 1; };
		virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
		void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

	private:
		std::vector<double> m_samples;

	};

}