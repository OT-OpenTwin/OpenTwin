// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ServiceBase.h"
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/ExpanderWidget.h"
#include "OTWidgets/TreeWidgetFilter.h"

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
