#pragma once
#pragma warning(disable : 4251)

// Foundation header
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Flags.h"
#include "OpenTwinFoundation/FoundationAPIExport.h"
#include "OpenTwinFoundation/FoundationTypes.h"

// std header
#include <string>
#include <list>

namespace ot {

	class OT_SERVICEFOUNDATION_API_EXPORT ContextMenuItemRole {
	public:
		enum itemRole {
			NoRole,
			SettingsReference,
			Clear,
			Custom
		};

		ContextMenuItemRole();
		ContextMenuItemRole(itemRole _role, const std::string& _roleText);
		ContextMenuItemRole(const ContextMenuItemRole& _other);
		ContextMenuItemRole& operator = (const ContextMenuItemRole& _other);
		virtual ~ContextMenuItemRole();

		itemRole role(void) const { return m_role; }
		const std::string& roleText(void) const { return m_roleText; }

		std::string roleToString(void) const;
		static std::string roleToString(itemRole _role);
		static itemRole stringToRole(const std::string& _role);
		

		void addToJsonObject(OT_rJSON_doc & _doc, OT_rJSON_val & _object) const;
		void setFromJsonObject(OT_rJSON_val & _object);

	private:
		itemRole		m_role;
		std::string		m_roleText;
	};

	// #########################################################################################################################################

	// #########################################################################################################################################

	// #########################################################################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT AbstractContextMenuItem {
	public:
		enum itemType {
			Seperator,
			Item
		};

		AbstractContextMenuItem(itemType _type);
		AbstractContextMenuItem(const AbstractContextMenuItem& _other);
		AbstractContextMenuItem& operator = (const AbstractContextMenuItem& _other);
		virtual ~AbstractContextMenuItem();

		// Getter

		itemType type(void) const { return m_type; }

		std::string typeToString(void) const;
		static std::string typeToString(itemType _type);
		static itemType stringToType(const std::string& _type);

		void addToJsonObject(OT_rJSON_doc & _doc, OT_rJSON_val & _object) const;
		static AbstractContextMenuItem * buildItemFromJsonObject(OT_rJSON_val & _object);

		virtual AbstractContextMenuItem * createCopy(void) const = 0;

		// Setter

		void setFromJsonObject(OT_rJSON_val & _object);

	

	protected:
		virtual void addInternalToJsonObject(OT_rJSON_doc & _doc, OT_rJSON_val & _object) const = 0;
		virtual void setInternalFromJsonObject(OT_rJSON_val & _object) = 0;
		itemType		m_type;

	private:
		AbstractContextMenuItem() = delete;
	};

	// #########################################################################################################################################

	// #########################################################################################################################################

	// #########################################################################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT ContextMenuSeperator : public AbstractContextMenuItem {
	public:
		ContextMenuSeperator();
		ContextMenuSeperator(const ContextMenuSeperator& _other);
		ContextMenuSeperator& operator = (const ContextMenuSeperator& _other);
		virtual ~ContextMenuSeperator();

		virtual AbstractContextMenuItem * createCopy(void) const override;

	protected:
		virtual void addInternalToJsonObject(OT_rJSON_doc & _doc, OT_rJSON_val & _object) const override;
		virtual void setInternalFromJsonObject(OT_rJSON_val & _object) override;
	};

	// #########################################################################################################################################

	// #########################################################################################################################################

	// #########################################################################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT ContextMenuItem : public AbstractContextMenuItem {
	public:

		ContextMenuItem(const std::string& _name, const std::string& _text, const std::string& _icon = std::string(), const ContextMenuItemRole& _role = ContextMenuItemRole());
		ContextMenuItem(const ContextMenuItem& _other);
		ContextMenuItem& operator = (const ContextMenuItem& _other);
		virtual ~ContextMenuItem();

		// #######################################################################################################

		// Getter

		const std::string& name(void) const { return m_name; }
		const std::string& text(void) const { return m_text; }
		const std::string& icon(void) const { return m_icon; }
		const ContextMenuItemRole& role(void) const { return m_role; }
		bool isChecked(void) const;
		bool isCheckable(void) const;

		virtual AbstractContextMenuItem * createCopy(void) const override;

		// #######################################################################################################

		// Setter

		void setChecked(bool _isChecked = true);
		void setCheckable(bool _isCheckable = true);

		// #######################################################################################################

		// Operators

		ContextMenuItem& operator + (itemFlag _flag);
		ContextMenuItem& operator - (itemFlag _flag);
		ContextMenuItem& operator + (itemCheckedState _checkedState);

	protected:
		virtual void addInternalToJsonObject(OT_rJSON_doc & _doc, OT_rJSON_val & _object) const override;
		virtual void setInternalFromJsonObject(OT_rJSON_val & _object) override;

	private:
		std::string						m_name;
		std::string						m_text;
		std::string						m_icon;
		ContextMenuItemRole				m_role;
		ot::Flags<itemFlag>				m_flags;
		ot::Flags<itemCheckedState>		m_checkState;
		ContextMenuItem() = delete;
	};

	// #########################################################################################################################################

	// #########################################################################################################################################

	// #########################################################################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT ContextMenu {
	public:
		ContextMenu(const std::string& _name);
		ContextMenu(const ContextMenu& _other);
		ContextMenu& operator = (const ContextMenu& _other);
		virtual ~ContextMenu();

		// #######################################################################################################

		// Getter

		bool hasItems(void) const;

		const std::string& name(void) const { return m_name; }

		const std::list<AbstractContextMenuItem *>& items(void) const { return m_items; }

		void addToJsonObject(OT_rJSON_doc & _doc, OT_rJSON_val & _object) const;

		// #######################################################################################################

		// Setter
		
		void setFromJsonObject(OT_rJSON_val & _object);

		ContextMenu& addItem(const ContextMenuSeperator& _item);

		ContextMenu& addItem(const ContextMenuItem& _item);

		ContextMenu& operator << (const ContextMenuSeperator& _item);

		ContextMenu& operator << (const ContextMenuItem& _item);

	private:

		std::string								m_name;
		std::list<AbstractContextMenuItem *>	m_items;
	};

}