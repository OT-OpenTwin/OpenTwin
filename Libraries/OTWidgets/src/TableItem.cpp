//! @file TableItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

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
