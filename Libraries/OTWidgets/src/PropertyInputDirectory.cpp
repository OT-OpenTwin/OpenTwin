//! @file PropertyInputDirectory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyDirectory.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PropertyInputDirectory.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

ot::PropertyInputDirectory::PropertyInputDirectory(const PropertyDirectory* _property)
	: PropertyInput(_property), m_text(QString::fromStdString(_property->path()))
{
	m_root = new QWidget;
	QHBoxLayout* rLay = new QHBoxLayout(m_root);

	m_edit = new LineEdit;
	m_edit->setToolTip(QString::fromStdString(_property->propertyTip()));
	rLay->addWidget(m_edit, 1);

	QPushButton* btnFind = new QPushButton("Search");
	rLay->addWidget(btnFind);

	if (_property->propertyFlags() & Property::HasMultipleValues) {
		m_edit->setText("...");
	}
	else {
		m_edit->setText(m_text);
	}

	this->connect(m_edit, &QLineEdit::editingFinished, this, &PropertyInputDirectory::slotChanged);
	this->connect(btnFind, &QPushButton::clicked, this, &PropertyInputDirectory::slotFind);
}

ot::PropertyInputDirectory::~PropertyInputDirectory() {
	delete m_edit;
}

void ot::PropertyInputDirectory::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_text.toStdString(), _allocator), _allocator);
}

QVariant ot::PropertyInputDirectory::getCurrentValue(void) const {
	return QVariant(m_edit->text());
}

QWidget* ot::PropertyInputDirectory::getQWidget(void) {
	return m_root;
}

void ot::PropertyInputDirectory::slotFind(void) {
	QString pth = QFileDialog::getExistingDirectory(m_edit, this->propertyTitle(), m_edit->text());
	if (!pth.isEmpty() && m_edit->text() != pth) {
		m_text = pth;
		m_edit->setText(pth);
		this->slotValueChanged();
	}
}

void ot::PropertyInputDirectory::slotChanged(void) {
	if (m_edit->text() != m_text) {
		m_text = m_edit->text();
		this->slotValueChanged();
	}
}