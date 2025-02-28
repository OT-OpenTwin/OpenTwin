//! @file PropertyInputFilePath.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyFilePath.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/FilePathEdit.h"
#include "OTWidgets/SignalBlockWrapper.h"
#include "OTWidgets/PropertyInputFilePath.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

// Qt header
#include <QtWidgets/qfiledialog.h>

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputFilePath> propertyInputFilePathRegistrar(ot::PropertyFilePath::propertyTypeString());

ot::PropertyInputFilePath::PropertyInputFilePath()
	: m_mode(PropertyFilePath::ReadFile)
{
	m_path = new FilePathEdit(FilePathEdit::OpenFileMode);
	
	this->connect(m_path, &FilePathEdit::fileChanged, this, qOverload<>(& PropertyInputFilePath::slotValueChanged));
}

ot::PropertyInputFilePath::~PropertyInputFilePath() {
	delete m_path;
}

void ot::PropertyInputFilePath::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_text.toStdString(), _allocator), _allocator);
}

QVariant ot::PropertyInputFilePath::getCurrentValue(void) const {
	return QVariant(m_text);
}

QWidget* ot::PropertyInputFilePath::getQWidget(void) {
	return m_path->getQWidget();
}

const QWidget* ot::PropertyInputFilePath::getQWidget(void) const {
	return m_path->getQWidget();
}

ot::Property* ot::PropertyInputFilePath::createPropertyConfiguration(void) const {
	ot::PropertyFilePath* newProperty = new ot::PropertyFilePath(this->data());

	newProperty->setBrowseMode(m_mode);
	newProperty->setPath(m_path->getFilePath().toStdString());

	return newProperty;
}

bool ot::PropertyInputFilePath::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyFilePath* actualProperty = dynamic_cast<const PropertyFilePath*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	SignalBlockWrapper sigBlock(m_path);

	m_mode = actualProperty->getBrowseMode();

	m_path->getLineEdit()->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_path->setFilePath("...");
	}
	else {
		m_path->setFilePath(m_text);
	}

	for (PropertyFilePath::FilterInfo info : actualProperty->getFilters()) {
		if (!m_filter.isEmpty()) m_filter.append(";;");
		m_filter.append(QString::fromStdString(info.text) + " (" + QString::fromStdString(info.extension) + ")");
	}
	m_path->getLineEdit()->setReadOnly(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	return true;
}

void ot::PropertyInputFilePath::focusPropertyInput(void) {
	m_path->getLineEdit()->setFocus();
}

void ot::PropertyInputFilePath::setCurrentFile(const QString& _file) {
	m_path->setFilePath(_file);
}

QString ot::PropertyInputFilePath::currentFile(void) const {
	return m_path->getFilePath();
}