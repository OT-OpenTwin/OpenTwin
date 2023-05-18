//! @file TypeConversion.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "OTQtWrapper/TypeConversion.h"

#include <QtCore/qstring.h>

QFont& ot::operator << (QFont& _lhv, const ot::Font& _rhv) {
	if (!_rhv.family().empty()) _lhv.setFamily(QString::fromStdString(_rhv.family()));
	_lhv.setPointSize(_rhv.size());
	_lhv.setBold(_rhv.isBold());
	_lhv.setItalic(_rhv.isItalic());
	return _lhv;
}

ot::Font& ot::operator << (ot::Font& _lhv, const QFont& _rhv) {
	if (!_rhv.family().isEmpty()) _lhv.setFamily(_rhv.family().toStdString());
	_lhv.setSize(_rhv.pointSize());
	_lhv.setBold(_rhv.bold());
	_lhv.setItalic(_rhv.italic());
	return _lhv;
}
