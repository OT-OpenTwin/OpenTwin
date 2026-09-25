// @otlicense
// File: TransmissionLine.h
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

 //Sevice Header
#include "CircuitElement.h"

class TransmissionLine : public CircuitElement {
	OT_DECL_NOCOPY(TransmissionLine)
public:
	TransmissionLine() = default;
	TransmissionLine(std::string impedance, std::string transmissionDelay, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~TransmissionLine();

	std::string type() const override { return "TransmissionLine"; }
	void initFromEntity(const std::shared_ptr<ot::EntityBlock>& _entity, const std::string& _editorName) override;

	//Getter
	const std::string getImpedance() const { return "Z0=" + this->m_impedance; }
	const std::string getTransmissionDelay() const { return "TD=" + this->m_transmissionDelay; }
	std::string getNetlistPrefix() const override { return "T"; }
	std::string getNetlistValue() const override { return getImpedance() + " " + getTransmissionDelay(); }
	bool isTransmissionLine() const override { return true; }
	std::vector<std::string> getPositivePoleNames() const override { return { "PositivePole1", "PositivePole2" }; }
	std::vector<std::string> getNegativePoleNames() const override { return { "NegativePole1", "NegativePole2" }; }

	// NGSpice T-line requires: port1+, port1-, port2+, port2- (interleaved, not grouped)
	std::vector<std::string> getOrderedPoleNames() const override {
		return { "PositivePole1", "NegativePole1", "PositivePole2", "NegativePole2" };
	}

	// T-line needs all 4 nodes even if some share the same number (e.g., both grounds = 0)
	bool allowDuplicateNodes() const override { return true; }

	//Setter
	void setImpedance(std::string impedance) { this->m_impedance = impedance; }
	void setTransmissionDelay(std::string transmissionDelay) { this->m_transmissionDelay = transmissionDelay; }

private:
	std::string m_impedance;
	std::string m_transmissionDelay;
};