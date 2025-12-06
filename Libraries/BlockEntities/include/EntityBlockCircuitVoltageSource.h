// @otlicense
// File: EntityBlockCircuitVoltageSource.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
#include "EntityBlockCircuitElement.h"
class __declspec(dllexport) EntityBlockCircuitVoltageSource : public EntityBlockCircuitElement {
	public:
		EntityBlockCircuitVoltageSource() : EntityBlockCircuitVoltageSource(0, nullptr, nullptr, nullptr) {};
		EntityBlockCircuitVoltageSource(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);

		static std::string className() { return "EntityBlockCircuitVoltageSource"; }
		virtual std::string getClassName(void) const override { return EntityBlockCircuitVoltageSource::className(); };
		virtual entityType getEntityType(void) const override { return TOPOLOGY; };
		virtual void createProperties() override;
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

		virtual ot::GraphicsItemCfg* createBlockCfg() override;
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

		void addStorageData(bsoncxx::builder::basic::document& storage) override;
		void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

		
};