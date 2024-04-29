#pragma once

#include "OTCore/JSON.h"

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include <map>
#include <list>

class UserSettings;
namespace ot { class ServiceBase; }
namespace ot { class SettingsData; }
namespace ot { class SettingsGroup; }
namespace ot { class AbstractSettingsItem; }
namespace ak { class aOptionsDialog; }
namespace ak { class aOptionsGroup; }
namespace ak { class aAbstractOptionsItem; }

class SettingsItemOwner {
public:
	SettingsItemOwner() : m_item(nullptr), m_owner(nullptr), m_uiItem(nullptr), m_isInternal(false) {}
	SettingsItemOwner(const SettingsItemOwner& _other);
	SettingsItemOwner(ot::ServiceBase * _owner, ot::AbstractSettingsItem * _item, ak::aAbstractOptionsItem * _uiItem);
	virtual ~SettingsItemOwner() {}
	SettingsItemOwner& operator = (const SettingsItemOwner& _other);

	void setItem(ot::AbstractSettingsItem * _item) { m_item = _item; }
	void setOwner(ot::ServiceBase * _owner) { m_owner = _owner; }
	void setUiItem(ak::aAbstractOptionsItem * _item) { m_uiItem = _item; }

	ot::AbstractSettingsItem * item(void) { return m_item; }
	ot::ServiceBase * owner(void) { return m_owner; }
	ak::aAbstractOptionsItem * uiItem(void) { return m_uiItem; }

private:
	ot::AbstractSettingsItem *	m_item;
	ot::ServiceBase *			m_owner;
	ak::aAbstractOptionsItem *	m_uiItem;
	bool						m_isInternal;
};

// #########################################################################################################################################

// #########################################################################################################################################

// #########################################################################################################################################

class UserSettings : public QObject {
	Q_OBJECT
public:
	static UserSettings * instance(void);

	void showDialog(void);

	void showDialog(const QString& _group);

	// #######################################################################################

	// Data management

	void clear(void);

	void addFromService(ot::ServiceBase * _sender, ot::JsonDocument& _document);

	void updateUiServiceSettings(ot::SettingsData * _data);

	void updateViewerSettings(ot::SettingsData * _data);

	void initializeData(void);

	// #######################################################################################

private Q_SLOTS:
	void slotItemChanged(ak::aAbstractOptionsItem * _item);

private:

	void uiServiceSettingsChanged(ot::AbstractSettingsItem * _item);

	void eraseRootGroups(ot::SettingsData * _data);

	ak::aOptionsGroup * parseFromSettingsGroup(ot::ServiceBase * _owner, ot::SettingsGroup * _group);

	ak::aAbstractOptionsItem * parseFromSettingsItem(ot::AbstractSettingsItem * _item);

	ot::SettingsData *							m_uiServiceSettings;
	ot::SettingsData *							m_viewerSettings;
	ak::aOptionsDialog *						m_dialog;

	std::map<
		ot::ServiceBase *, ot::SettingsData *>	m_serviceToSettingsMap;

	std::map<ak::aAbstractOptionsItem *,
		SettingsItemOwner>						m_uiToOwnerMap;

	UserSettings();
	UserSettings(UserSettings&) = delete;
	UserSettings& operator = (UserSettings&) = delete;
};
