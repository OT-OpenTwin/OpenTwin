// @otlicense
// File: PropertyInputPath.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PathBrowseEdit.h"
#include "OTWidgets/PropertyInputPath.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

// Qt header
#include <QtWidgets/qfiledialog.h>

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputPath> propertyInputFilePathRegistrar(ot::PropertyPath::propertyTypeString());

ot::PropertyInputPath::PropertyInputPath() {
	m_path = new PathBrowseEdit(PathBrowseMode::ReadFile);
	
	this->connect(m_path, &PathBrowseEdit::pathChanged, this, qOverload<>(&PropertyInputPath::slotValueChanged));
}

ot::PropertyInputPath::~PropertyInputPath() {
	delete m_path;
}

void ot::PropertyInputPath::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_path->getPath().toStdString(), _allocator), _allocator);
}

QVariant ot::PropertyInputPath::getCurrentValue(void) const {
	return QVariant(m_path->getPath());
}

QWidget* ot::PropertyInputPath::getQWidget(void) {
	return m_path->getQWidget();
}

const QWidget* ot::PropertyInputPath::getQWidget(void) const {
	return m_path->getQWidget();
}

ot::Property* ot::PropertyInputPath::createPropertyConfiguration(void) const {
	ot::PropertyPath* newProperty = new ot::PropertyPath(this->data());

	newProperty->setBrowseMode(m_path->getBrowseMode());
	newProperty->setPath(m_path->getPath().toStdString());

	return newProperty;
}

bool ot::PropertyInputPath::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyPath* actualProperty = dynamic_cast<const PropertyPath*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	QSignalBlocker sigBlock(m_path);

	m_path->setBrowseMode(actualProperty->getBrowseMode());

	m_path->getLineEdit()->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_path->setPath("...");
	}
	else {
		m_path->setPath(QString::fromStdString(actualProperty->getPath()));
	}

	std::string filterString;
	for (PropertyPath::FilterInfo info : actualProperty->getFilters()) {
		if (!filterString.empty()) {
			filterString.append(";;");
		}

		filterString.append(info.text + " (" + info.extension + ")");
	}
	m_path->setFileFilter(QString::fromStdString(filterString));
	m_path->getLineEdit()->setReadOnly(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	return true;
}

void ot::PropertyInputPath::focusPropertyInput(void) {
	m_path->getLineEdit()->setFocus();
}

void ot::PropertyInputPath::setCurrentFile(const QString& _file) {
	m_path->setPath(_file);
}

QString ot::PropertyInputPath::currentFile(void) const {
	return m_path->getPath();
}