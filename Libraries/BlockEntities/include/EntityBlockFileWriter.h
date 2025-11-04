// @otlicense
// File: EntityBlockFileWriter.h
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
#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockFileWriter : public EntityBlock
{
public:
	EntityBlockFileWriter() : EntityBlockFileWriter(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockFileWriter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);

	static std::string className() { return "EntityBlockFileWriter"; }
	virtual std::string getClassName(void) const override { return EntityBlockFileWriter::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual void createProperties() override;
	const std::string& getHeadline();
	const std::string& getFileName();
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	const ot::Connector& getConnectorInput() const { return m_inputConnector; }
	const std::string& getFileModeAppend() const { return m_fileModeAppend; }
	const std::string& getFileModeOverride() const { return m_fileModeOverride; }

	static std::string getIconName(){ return "TextFile.svg"; }
	const std::string getFileType();

private:
	ot::Connector m_inputConnector;
	const std::string m_fileModeAppend = "Append";
	const std::string m_fileModeOverride = "Override";

};
