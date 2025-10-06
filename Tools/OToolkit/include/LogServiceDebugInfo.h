//! @file LogServiceDebugInfo.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/ServiceDebugInformation.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qsettings.h>

class QTableWidgetItem;
namespace ot { class Table; }

class LogServiceDebugInfo : public QObject {
	Q_OBJECT
public:
	LogServiceDebugInfo();
	virtual ~LogServiceDebugInfo();
	
	QWidget* getRootWidget() const { return m_root; };

	void restoreSettings(QSettings& _settings);
	void saveSettings(QSettings& _settings);
	
	void appendServiceDebugInfo(const ot::ServiceDebugInformation& _info);

private Q_SLOTS:
	void slotClear();
	void slotItemDoubleClicked(QTableWidgetItem* _item);

private:
	QWidget* m_root;

	ot::Table* m_table;
};