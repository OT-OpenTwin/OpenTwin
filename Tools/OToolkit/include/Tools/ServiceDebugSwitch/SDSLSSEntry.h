// @otlicense
// File: SDSLSSEntry.h
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

#pragma once

// OpenTwin header
#include "OTCore/ServiceBase.h"
#include "OTWidgets/Widgets/WidgetBase.h"
#include "OTWidgets/Widgets/ExpanderWidget.h"
#include "OTWidgets/Widgets/TreeWidgetFilter.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <list>

class QTreeWidgetItem;

class SDSLSSEntry : public QObject, public ot::WidgetBase {
	Q_OBJECT
	OT_DECL_NOCOPY(SDSLSSEntry)
	OT_DECL_NOMOVE(SDSLSSEntry)
	OT_DECL_NODEFAULT(SDSLSSEntry)
private:
	struct ServiceDebugInfo {
		std::string serviceName;
		bool isDebug;
	};
	typedef std::list<ServiceDebugInfo> ServiceDebugInfoList;

public:
	SDSLSSEntry(const ot::ServiceBase& _serviceInfo, QWidget* _parent);
	virtual ~SDSLSSEntry() = default;

	virtual QWidget* getQWidget() { return m_expander; };
	virtual const QWidget* getQWidget() const { return m_expander; };

	const ot::ServiceBase& getServiceInfo() const { return m_serviceInfo; };

	void expandEntry() { m_expander->expand(); }

public Q_SLOTS:
	void refreshData();

private Q_SLOTS:
	void workerFinished(const ServiceDebugInfoList& _services);
	void itemDoubleClicked(QTreeWidgetItem* _item, int _column);

private:
	void workerLoadData();

	ot::ServiceBase m_serviceInfo;
	ot::ExpanderWidget* m_expander;
	ot::TreeWidget* m_tree;
};
