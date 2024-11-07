#pragma once

#include "OTServiceFoundation/ContextMenu.h"

#include <akCore/akCore.h>
#include <akCore/aNotifier.h>

#include <string>
#include <list>
#include <map>

namespace ot {
	class ServiceBase;
}

class ContextMenuEntry {
public:
	ContextMenuEntry(const std::string& _name, ak::ID _id, const std::string& _text, const std::string& _role = std::string(), const std::string& _roleText = std::string(), const std::string& _icon = std::string());
	ContextMenuEntry(const ContextMenuEntry& _other);
	ContextMenuEntry& operator = (const ContextMenuEntry& _other);
	virtual ~ContextMenuEntry();

	void setId(ak::ID _id) { m_id = _id; }

	const std::string& text(void) const { return m_text; }
	const std::string& name(void) const { return m_name; }
	const std::string& icon(void) const { return m_icon; }
	const std::string& role(void) const { return m_role; }
	const std::string& roleText(void) const { return m_roleText; }
	ak::ID id(void) const { return m_id; }
	
private:
	std::string		m_text;
	std::string		m_name;
	std::string		m_icon;
	std::string		m_role;
	std::string		m_roleText;
	ak::ID			m_id;

	ContextMenuEntry() = delete;
};

// ######################################################################################################################################

// ######################################################################################################################################

// ######################################################################################################################################

class ContextMenuItem {
public:
	ContextMenuItem(ot::ServiceBase * _service, ak::UID _creatorUid, const std::string& _name, ak::UID _uid);
	ContextMenuItem(const ContextMenuItem& _other);
	ContextMenuItem& operator = (const ContextMenuItem& _other);
	virtual ~ContextMenuItem();

	// #####################################################################################################

	// Setter

	ContextMenuEntry * addEntry(ContextMenuEntry * _entry);

	ContextMenuEntry * addEntry(ot::ContextMenuItem * _item);

	ContextMenuEntry * addEntry(const std::string& _name, const std::string& _text, const std::string& _role, const std::string& _icon = std::string());

	void addSeperator(void);
	  
	void setIsViewerContext(bool _isViewer = true) { m_isViewerContext = _isViewer; }
	
	// #####################################################################################################

	// Getter

	const ContextMenuEntry * findItemById(ak::ID _id);

	bool isViewerContext(void) const { return m_isViewerContext; }
	ak::UID creatorUid(void) const { return m_creatorUid; }
	ot::ServiceBase * service(void) { return m_service; }
	const std::string& name(void) const { return m_name; }
	ak::UID uid(void) const { return m_uid; }
	const std::list<ContextMenuEntry *>& items(void) const { return m_items; }

private:

	bool							m_isViewerContext;
	ak::UID							m_creatorUid;
	std::list<ContextMenuEntry *>	m_items;
	ot::ServiceBase *				m_service;
	ak::UID							m_uid;
	std::string						m_name;
};

// ######################################################################################################################################

// ######################################################################################################################################

// ######################################################################################################################################

class ContextMenuManager : public ak::aNotifier {
public:
	ContextMenuManager();
	virtual ~ContextMenuManager();

	virtual void notify(ak::UID _sender, ak::eventType _event, int _info1, int _info2);

	ContextMenuItem * createItem(ot::ServiceBase * _creator, ak::UID _creatorUid, ak::UID _controlUid, const ot::ContextMenu& _menu);

	ContextMenuItem * createItem(ot::ServiceBase * _creator, ak::UID _creatorUid, const std::string& _name, ak::UID _controlUid, const std::list<ContextMenuEntry *>& _entries = std::list<ContextMenuEntry *>());

	void serviceDisconnected(ot::ServiceBase * _service);

	void uiItemDestroyed(ak::UID _itemUid);

private:
	std::list<ContextMenuItem *> * getOrCreateDataList(ot::ServiceBase * _service);

	bool hasDataListEntry(ot::ServiceBase * _service);

	ContextMenuItem * findItemByUid(ak::UID _uid);

	std::map<ot::ServiceBase *, std::list<ContextMenuItem *> *>		m_serviceData;
	std::map<ak::UID, ContextMenuItem *>							m_uiToDataMap;

};
