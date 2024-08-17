#pragma once

// uiCore header
#include <akCore/globalDataTypes.h>
#include <akCore/aNotifier.h>

// Qt header
#include <qstring.h>
#include <qicon.h>

// open twin header
#include "OTCore/Flags.h"
#include "OTGui/GuiTypes.h"

class AppBase;
class LockManager;

class ToolBar : public ak::aNotifier {
public:

	ToolBar(AppBase * _owner);

	virtual void notify(
		ak::UID			_sender,
		ak::eventType	_event,
		int				_info1,
		int				_info2
	) override;
	
	// ###################################################################################

	ak::UID addPage(ak::UID _creator, const QString & _pageName);

	ak::UID addGroup(ak::UID _creator, ak::UID _page, const QString & _groupName);

	ak::UID addSubGroup(ak::UID _creator, ak::UID _group, const QString & _subGroupName);

	ak::UID addToolButton(ak::UID _creator, ak::UID _container, const QIcon & _icon, const QString & _title);

	ak::UID addToolButton(ak::UID _creator, ak::UID _container, const QString & _iconName, const QString & _iconPath, const QString & _title);

	ak::UID addToolButton(ak::UID _creator, ak::UID _container, const QString & _iconName, const QString & _iconPath, const QString & _title, ak::aNotifier * _notifier);

	ak::UID addCheckBox(ak::UID _creator, ak::UID _container, const QString & _text, bool _initialState);

	ak::UID addNiceLineEdit(ak::UID _creator, ak::UID _container, const QString & _title, const QString & _initialState);

	void addDefaultControlsToLockManager(LockManager * _lockManger, ot::LockTypeFlags& _flags);

private:

	struct structView {
		ak::UID				page;
		ak::UID				gUserInterface;
		ak::UID				gUserInterface_aSettings;
	};

	struct structFile {
		ak::UID				page;
		ak::UID				gDefault;
		ak::UID				gDefault_aExit;
		ak::UID				gDefault_aSettings;
		ak::UID				gDefault_aImport;
		ak::UID				gDefault_aGroup;
	};

	AppBase *			m_owner;

	structFile			m_file;
	structView			m_view;

	ToolBar() = delete;
	ToolBar(ToolBar&) = delete;
	ToolBar& operator = (ToolBar&) = delete;
};