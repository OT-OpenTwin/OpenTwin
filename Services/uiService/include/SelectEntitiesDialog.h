//! @file SelectEntitiesDialog.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/NavigationTreeItem.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtCore/qstringlist.h>

// std header
#include <list>

class QVBoxLayout;
class QHBoxLayout;

namespace ot { 
	class TreeWidget;
}

class SelectEntitiesDialog : public ot::Dialog {
	Q_OBJECT
public:
	SelectEntitiesDialog(const std::list<ot::NavigationTreeItem>& _availableItems, const std::list<ot::NavigationTreeItem>& _selectedItems, QWidget* _parent = (QWidget*)nullptr);
	~SelectEntitiesDialog();



private slots:
	void slotApply(void);
	void slotCancel(void);

private:
	QStringList m_lst;

	ot::TreeWidget* m_available;
	ot::TreeWidget* m_selected;
};