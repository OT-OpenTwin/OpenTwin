//! @file PropertyInputDirectory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyDirectory.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PropertyInputDirectory.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputDirectory> propertyInputDirectoryRegistrar(ot::PropertyDirectory::propertyTypeString());

ot::PropertyInputDirectory::PropertyInputDirectory()
{
	m_root = new QWidget;
	QHBoxLayout* rLay = new QHBoxLayout(m_root);

	m_edit = new LineEdit;
	rLay->addWidget(m_edit, 1);

	m_find = new PushButton("Search");
	rLay->addWidget(m_find);
	
	this->connect(m_edit, &QLineEdit::editingFinished, this, &PropertyInputDirectory::slotChanged);
	this->connect(m_find, &QPushButton::clicked, this, &PropertyInputDirectory::slotFind);
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

const QWidget* ot::PropertyInputDirectory::getQWidget(void) const {
	return m_root;
}

void ot::PropertyInputDirectory::slotFind(void) {
	QString pth = QFileDialog::getExistingDirectory(m_edit, QString::fromStdString(this->data().getPropertyTitle()), m_edit->text());
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

ot::Property* ot::PropertyInputDirectory::createPropertyConfiguration(void) const {
	ot::PropertyDirectory* newProperty = new ot::PropertyDirectory(this->data());
	
	newProperty->setPath(m_edit->text().toStdString());

	return newProperty;
}

bool ot::PropertyInputDirectory::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyDirectory* actualProperty = dynamic_cast<const PropertyDirectory*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_edit->blockSignals(true);

	m_text = QString::fromStdString(actualProperty->getPath());

	m_edit->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_edit->setText("...");
	}
	else {
		m_edit->setText(m_text);
	}
	m_edit->setReadOnly(this->data().getPropertyFlags() & Property::IsReadOnly);
	m_find->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	m_edit->blockSignals(false);

	return true;
}

void ot::PropertyInputDirectory::focusPropertyInput(void) {
	m_edit->setFocus();
}

void ot::PropertyInputDirectory::setCurrentDirectory(const QString& _dir) {
	m_edit->setText(_dir);
}

QString ot::PropertyInputDirectory::currentDirectory(void) const {
	return m_edit->text();
}