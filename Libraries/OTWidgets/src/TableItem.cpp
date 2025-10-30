// @otlicense
// File: TableItem.cpp
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

// OpenTwin header
#include "OTWidgets/TableItem.h"

ot::TableItem::TableItem(const QString& _text, const QString& _sortHint) : QTableWidgetItem(_text), m_sortHint(_sortHint) {}

ot::TableItem::TableItem(const QIcon& _icon, const QString& _text, const QString& _sortHint) : QTableWidgetItem(_icon, _text), m_sortHint(_sortHint) {}

bool ot::TableItem::operator<(const QTableWidgetItem& _other) const {
	QString otherTxt(_other.text());
	const TableItem* customItm = dynamic_cast<const TableItem*>(&_other);
	if (customItm) {
		if (!customItm->getSortHint().isEmpty()) {
			otherTxt = customItm->getSortHint();
		}
	}

	if (m_sortHint.isEmpty()) {
		return this->text() < otherTxt;
	}
	else {
		return m_sortHint < otherTxt;
	}
}
