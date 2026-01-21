// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/OnOffSlider.h"
#include "OTWidgets/TreeWidgetItem.h"

// Qt header
#include <QtCore/qobject.h>

class SDSServiceEntry : public QObject, public ot::TreeWidgetItem {
	Q_OBJECT
	OT_DECL_NOCOPY(SDSServiceEntry)
	OT_DECL_NOMOVE(SDSServiceEntry)
	OT_DECL_NODEFAULT(SDSServiceEntry)
public:
	enum class ColumnIndex : int {
		ServiceName,
		ToggleButton,
		ColumnCount
	};

	SDSServiceEntry(ot::TreeWidget* _tree, const std::string& _lssUrl, const std::string& _serviceName, bool _isDebug);

	ot::OnOffSlider* getToggleSlider() const { return m_toggleSlider; };

public Q_SLOTS:
	void toggle();

private Q_SLOTS:
	void debugToggled(bool _checked);

private:
	std::string m_lssUrl;
	std::string m_name;
	bool m_isDebug;
	
	ot::OnOffSlider* m_toggleSlider;

};