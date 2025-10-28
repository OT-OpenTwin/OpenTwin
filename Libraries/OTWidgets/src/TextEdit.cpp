// @otlicense

// OpenTwin header
#include "OTWidgets/TextEdit.h"

// Qt header
#include <QtGui/qfontmetrics.h>

ot::TextEdit::TextEdit(QWidget* _parent)
	: QTextEdit(_parent) 
{
	QFontMetrics f(this->font());
	this->setTabStopDistance(4 * f.horizontalAdvance(QChar(' ')));
}

ot::TextEdit::~TextEdit() {

}
