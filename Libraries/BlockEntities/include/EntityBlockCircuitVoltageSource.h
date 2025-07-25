#pragma once
#include "EntityBlockCircuitElement.h"
class __declspec(dllexport) EntityBlockCircuitVoltageSource : public EntityBlockCircuitElement {
	public:
		EntityBlockCircuitVoltageSource(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
		virtual std::string getClassName(void) override { return "EntityBlockCircuitVoltageSource"; };
		virtual entityType getEntityType(void) const override { return TOPOLOGY; };
		virtual void createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID) override;
		std::string getVoltage();
		std::string getFunction();
		virtual std::string getTypeAbbreviation() override;
		virtual std::string getFolderName() override;

		double getRotation();
		std::string getFlip();
		std::vector<std::string> getPulseParameters();
		std::vector<std::string> getSinParameters();
		std::vector<std::string> getExpParameters();
		std::string getAmplitude();

		virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
		virtual bool updateFromProperties(void) override;
		const ot::Connector getPositiveConnector() const { return m_positiveConnector; }
		const ot::Connector getNegativeConnector() const { return m_negativeConnector; }

	private:
	
		ot::Connector m_positiveConnector;
		ot::Connector m_negativeConnector;


		/*void createTransformProperties();*/
		void createTRANProperties();
		void createACProperties();
		void createDCProperties();
		

		void createPULSEProperties();
		void createSINProperties();
		void createEXPProperties();
		void createAmplitudeProperties();
		
		bool SetVisiblePULSEProperties(bool visible);
		bool SetVisibleSINProperties(bool visible);
		bool SetVisibleEXPProperties(bool visible);

		void AddStorageData(bsoncxx::builder::basic::document& storage) override;
		void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

		
};