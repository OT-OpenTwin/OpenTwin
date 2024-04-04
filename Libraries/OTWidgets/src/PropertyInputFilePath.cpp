//! @file PropertyInputFilePath.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyFilePath.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PropertyInputFilePath.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

ot::PropertyInputFilePath::PropertyInputFilePath(const PropertyFilePath* _property)
	: PropertyInput(_property), m_mode(_property->browseMode()), m_text(QString::fromStdString(_property->path()))
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

	for (PropertyFilePath::FilterInfo info : _property->filters()) {
		if (!m_filter.isEmpty()) m_filter.append(";;");
		m_filter.append(QString::fromStdString(info.text) + " (" + QString::fromStdString(info.extension) + ")");
	}

	this->connect(m_edit, &QLineEdit::editingFinished, this, &PropertyInputFilePath::slotChanged);
	this->connect(btnFind, &QPushButton::clicked, this, &PropertyInputFilePath::slotFind);
}

ot::PropertyInputFilePath::~PropertyInputFilePath() {
	delete m_edit;
}

void ot::PropertyInputFilePath::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_text.toStdString(), _allocator), _allocator);
}

QVariant ot::PropertyInputFilePath::getCurrentValue(void) const {
	return QVariant(m_text);
}

QWidget* ot::PropertyInputFilePath::getQWidget(void) {
	return m_root;
}

void ot::PropertyInputFilePath::slotFind(void) {
	QString pth;
	if (m_mode == PropertyFilePath::ReadFile) {
		pth = QFileDialog::getOpenFileName(m_edit, this->propertyTitle(), m_edit->text(), m_filter);
	} else {
		pth = QFileDialog::getSaveFileName(m_edit, this->propertyTitle(), m_edit->text(), m_filter);
	}
	if (!pth.isEmpty() && m_edit->text() != pth) {
		m_text = pth;
		m_edit->setText(pth);
		this->slotValueChanged();
	}
}

void ot::PropertyInputFilePath::slotChanged(void) {
	if (m_edit->text() != m_text) {
		m_text = m_edit->text();
		this->slotValueChanged();
	}
}

ot::Property* ot::PropertyInputFilePath::createPropertyConfiguration(void) const {
	ot::PropertyFilePath* newProperty = new ot::PropertyFilePath;
	newProperty->setPropertyName(this->propertyName());
	newProperty->setPropertyTitle(this->propertyTitle().toStdString());
	newProperty->setPropertyTip(this->propertyTip().toStdString());
	newProperty->setPropertyFlags(this->propertyFlags());

	newProperty->setPath(m_edit->text().toStdString());

	return newProperty;
}

void ot::PropertyInputFilePath::setCurrentFile(const QString& _file) {
	m_edit->setText(_file);
}

QString ot::PropertyInputFilePath::currentFile(void) const {
	return m_edit->text();
}