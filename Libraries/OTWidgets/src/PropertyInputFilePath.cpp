//! @file PropertyInputFilePath.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyFilePath.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PropertyInputFilePath.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputFilePath> propertyInputFilePathRegistrar(OT_PROPERTY_TYPE_FilePath);

ot::PropertyInputFilePath::PropertyInputFilePath()
	: m_mode(PropertyFilePath::ReadFile)
{
	m_root = new QWidget;
	QHBoxLayout* rLay = new QHBoxLayout(m_root);

	m_edit = new LineEdit;
	
	rLay->addWidget(m_edit, 1);

	m_find = new PushButton("Search");
	rLay->addWidget(m_find);

	this->connect(m_edit, &QLineEdit::editingFinished, this, &PropertyInputFilePath::slotChanged);
	this->connect(m_find, &QPushButton::clicked, this, &PropertyInputFilePath::slotFind);
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
		pth = QFileDialog::getOpenFileName(m_edit, QString::fromStdString(this->data().getPropertyTitle()), m_edit->text(), m_filter);
	} else {
		pth = QFileDialog::getSaveFileName(m_edit, QString::fromStdString(this->data().getPropertyTitle()), m_edit->text(), m_filter);
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
	ot::PropertyFilePath* newProperty = new ot::PropertyFilePath(this->data());

	newProperty->setBrowseMode(m_mode);
	newProperty->setPath(m_edit->text().toStdString()); 

	return newProperty;
}

bool ot::PropertyInputFilePath::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyFilePath* actualProperty = dynamic_cast<const PropertyFilePath*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_edit->blockSignals(true);

	m_mode = actualProperty->getBrowseMode();

	m_edit->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_edit->setText("...");
	}
	else {
		m_edit->setText(m_text);
	}

	for (PropertyFilePath::FilterInfo info : actualProperty->getFilters()) {
		if (!m_filter.isEmpty()) m_filter.append(";;");
		m_filter.append(QString::fromStdString(info.text) + " (" + QString::fromStdString(info.extension) + ")");
	}
	m_edit->setReadOnly(!(this->data().getPropertyFlags() & Property::IsReadOnly));
	m_find->setEnabled(this->data().getPropertyFlags() & Property::IsReadOnly);

	m_edit->blockSignals(false);

	return true;
}

void ot::PropertyInputFilePath::setCurrentFile(const QString& _file) {
	m_edit->setText(_file);
}

QString ot::PropertyInputFilePath::currentFile(void) const {
	return m_edit->text();
}