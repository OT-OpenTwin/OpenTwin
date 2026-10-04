// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/EntityContainer.h"

namespace ot
{
	class EntityMDFChannelData;

	class OT_MODELENTITIES_API_EXPORT EntityMDFChannel : public EntityContainer
	{
		OT_DECL_NOCOPY(EntityMDFChannel)
		OT_DECL_NOMOVE(EntityMDFChannel)
	public:
		EntityMDFChannel() : EntityMDFChannel(0, nullptr, nullptr, nullptr) {};
		explicit EntityMDFChannel(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);
		virtual ~EntityMDFChannel() = default;

		virtual entityType getEntityType() const override { return TOPOLOGY; };
		static std::string className() { return "EntityMDFChannel"; };
		virtual std::string getClassName() const override { return EntityMDFChannel::className(); };
		virtual bool getEntityBox(double&, double&, double&, double&, double&, double&) override { return false; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Data access

		void setDataEntityID(UID _dataID) { if (m_dataID != _dataID) { m_dataID = _dataID; setModified(); } };
		UID getDataEntityID() const { return m_dataID; };

		//! @brief Set the samples of the channel data.
		//! This methods requires a valid data entity id to be set.
		//! This operation can only be performed if the entity observer is set.
		//! @throw ot::Exception::ObjectNotFound if the data entity could not be loaded.
		void setSamples(std::vector<double>&& _samples);

		//! @brief Get the samples of the channel data.
		//! This methods requires a valid data entity id to be set.
		//! This operation can only be performed if the entity observer is set.
		//! @throw ot::Exception::ObjectNotFound if the data entity could not be loaded.
		//! @return Reference to the samples of the channel data.
		const std::vector<double>& getSamples();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Property access

		virtual bool updateFromProperties();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Serialization

		virtual std::string serialiseAsJSON() override;
		virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Serialization

	protected:
		virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
		void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Data

	private:
		UID m_dataID = ot::invalidUID;
		EntityMDFChannelData* m_data = nullptr;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

		void resetDataEntity();
		OT_DECL_NODISCARD bool ensureDataEntityLoaded();

	};

}