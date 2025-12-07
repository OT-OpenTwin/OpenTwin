// @otlicense
// File: EntityBlockDecoLabel.h
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

#include "OTGui/Font.h"
#include "EntityBlockDecoration.h"

class OT_BLOCKENTITIES_API_EXPORT EntityBlockDecoLabel : public EntityBlockDecoration {
public:
	EntityBlockDecoLabel() : EntityBlockDecoLabel(0, nullptr, nullptr, nullptr) {};
	EntityBlockDecoLabel(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);

	static std::string className() { return "EntityBlockDecoLabel"; }
	virtual std::string getClassName(void) const override { return EntityBlockDecoLabel::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsConnectionCfg::ConnectionShape getDefaultConnectionShape() const override { return ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine; };
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

	virtual void createProperties() override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data accessors

	void setText(const std::string& _text);
	std::string getText() const;

	void setTextPainter(const ot::Painter2D* _painter);
	const ot::Painter2D* getTextPainter() const;

	void setFontFamily(ot::FontFamily _fontFamily);
	void setFontFamily(const std::string& _fontFamily);
	std::string getFontFamily() const;

	void setFontSize(int _fontSize);
	int getFontSize() const;

	void setBold(bool _bold);
	bool getBold() const;

	void setItalic(bool _italic);
	bool getItalic() const;

	void setFont(const ot::Font& _font);
	ot::Font getFont() const;

	void setAlignment(ot::Alignment _alignment);
	ot::Alignment getAlignment() const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Property accessors

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;
};