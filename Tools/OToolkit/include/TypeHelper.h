// @otlicense
// File: TypeHelper.h
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

// Toolkit API header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qobject.h>

class QVBoxLayout;

namespace ot {
	class CheckBox;
	class LineEdit;
	class ComboBox;
}

class TypeHelper : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	TypeHelper() = default;
	virtual ~TypeHelper() = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString getToolName(void) const override { return QString("Type Helper"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

private Q_SLOTS:
	void slotNumberConvUintToInt();
	void slotNumberConvIntToUint();
	
	void slotNumverConvBaseFromDec();
	void slotNumverConvBaseFromHex();
	void slotNumverConvBaseFromBin();

private:
	void createNumberConversion(QVBoxLayout* _layout);

	QString tidyHex(const QString& _input);
	QString tidyBin(const QString& _input);

	struct NumberConv {
		ot::LineEdit* uIntInput = nullptr;
		ot::LineEdit* intOutput = nullptr;
		ot::ComboBox* uIntInputMode = nullptr;

		ot::LineEdit* intInput = nullptr;
		ot::LineEdit* uIntOutput = nullptr;
		ot::ComboBox* intInputMode = nullptr;

		ot::LineEdit* numToHexInput = nullptr;
		ot::LineEdit* numToHexOutput = nullptr;

		ot::LineEdit* baseConvDec = nullptr;
		ot::LineEdit* baseConvHex = nullptr;
		ot::LineEdit* baseConvBin = nullptr;
	};
	NumberConv m_numberConv;

};
