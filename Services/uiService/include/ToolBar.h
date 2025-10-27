#pragma once

// uiCore header
#include <akCore/globalDataTypes.h>
#include <akCore/aNotifier.h>

// Qt header
#include <qstring.h>
#include <qicon.h>

// open twin header
#include "OTSystem/Flags.h"
#include "OTGui/GuiTypes.h"
#include <qvectornd.h>

class AppBase;
class LockManager;

namespace tt { class Page; }

class ToolBar : public ak::aNotifier {
public:

	ToolBar(AppBase * _owner);

	virtual void notify(
		ot::UID			_sender,
		ak::eventType	_event,
		int				_info1,
		int				_info2
	) override;
	
	tt::Page* getStartPage(void);

	// ###################################################################################

	ot::UID addPage(ot::UID _creator, const QString & _pageName);

	ot::UID addGroup(ot::UID _creator, ot::UID _page, const QString & _groupName);

	ot::UID addSubGroup(ot::UID _creator, ot::UID _group, const QString & _subGroupName);

	ot::UID addToolButton(ot::UID _creator, ot::UID _container, const QIcon & _icon, const QString & _title);

	ot::UID addToolButton(ot::UID _creator, ot::UID _container, const QString & _iconName, const QString & _iconPath, const QString & _title);

	ot::UID addToolButton(ot::UID _creator, ot::UID _container, const QString & _iconName, const QString & _iconPath, const QString & _title, ak::aNotifier * _notifier);

	ot::UID addCheckBox(ot::UID _creator, ot::UID _container, const QString & _text, bool _initialState);

	ot::UID addNiceLineEdit(ot::UID _creator, ot::UID _container, const QString & _title, const QString & _initialState);

	void addDefaultControlsToLockManager(LockManager * _lockManger, ot::LockTypes& _flags);

private:

	struct structView {
		ot::UID				page;
		ot::UID				gUserInterface;
		ot::UID				gUserInterface_aSettings;
	};

	struct structFile {
		ot::UID				page;
		ot::UID				gDefault;
		ot::UID				gDefault_aExit;
		ot::UID				gDefault_aSettings;
		ot::UID				gDefault_aImport;
		ot::UID				gDefault_aGroup;
		ot::UID				gDefault_aExportLog;
	};

	AppBase *			m_owner;

	structFile			m_file;
	structView			m_view;

	ToolBar() = delete;
	ToolBar(ToolBar&) = delete;
	ToolBar& operator = (ToolBar&) = delete;
};