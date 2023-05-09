//! @file LineEdit.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "OTWidgets/LineEdit.h"

ot::LineEdit::LineEdit(QWidget* _parentWidget) : QLineEdit(_parentWidget) {}

ot::LineEdit::LineEdit(const QString& _initialText, QWidget* _parentWidget) : QLineEdit(_initialText, _parentWidget) {}
