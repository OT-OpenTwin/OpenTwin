// @otlicense
// File: BehavioralSource.h
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

// Service Header
#include "CircuitElement.h"

class BehavioralSource : public CircuitElement {
	OT_DECL_NOCOPY(BehavioralSource)
public:
	BehavioralSource() = default;
	BehavioralSource(std::string sourceType, std::string expression, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~BehavioralSource() override = default;

	std::string type() const override { return "BehavioralSource"; }
	void initFromEntity(const std::shared_ptr<ot::EntityBlock>& _entity, const std::string& _editorName) override;

	// Getter
	const std::string& getSourceType() const { return m_sourceType; }
	const std::string& getExpression() const { return m_expression; }
	std::string getNetlistPrefix() const override { return "B"; }
	std::string getNetlistValue() const override;

	// Setter
	void setSourceType(const std::string& sourceType) { m_sourceType = sourceType; }
	void setExpression(const std::string& expression) { m_expression = expression; }

private:
	std::string m_sourceType = "V";
	std::string m_expression;
};
