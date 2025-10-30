// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <list>

class QWidget;
namespace tt { class Group; }

namespace ot {

	class ToolButton;
	class TabToolBarPage;
	class TabToolBarSubGroup;

	class OT_WIDGETS_API_EXPORT TabToolBarGroup {
		OT_DECL_NOCOPY(TabToolBarGroup)
		OT_DECL_NODEFAULT(TabToolBarGroup)
	public:
		TabToolBarGroup(TabToolBarPage* _parentPage, tt::Group* _group, const std::string& _name);
		virtual ~TabToolBarGroup();

		const std::string& getName(void) const { return m_name; };

		void setParentTabToolBarPage(TabToolBarPage* _parentTabToolBarPage) { m_parentPage = _parentTabToolBarPage; };
		TabToolBarPage* getParentTabToolBarPage(void) { return m_parentPage; };
		const TabToolBarPage* getParentTabToolBarPage(void) const { return m_parentPage; };

		tt::Group* getGroup(void) const { return m_group; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Child management
		
		//! @brief Adds a new sub group to the TabToolBar group and returns it.
		//! @param _subGroupName Name/Title of the sub group.
		//! @param _returnExisting If true the sub group will be returned if it already exists. Otherwise nullptr will be returned in this case.
		TabToolBarSubGroup* addSubGroup(const std::string& _subGroupName, bool _returnExisting = false);

		//! @brief Removes the specified sub group from the lists.
		//! Caller keeps ownership of the sub group.
		void forgetSubGroup(TabToolBarSubGroup* _subGroup);

		TabToolBarSubGroup* findSubGroup(const std::string& _subGroupName);
		const TabToolBarSubGroup* findSubGroup(const std::string& _subGroupName) const;
		bool hasSubGroup(const std::string& _subGroupName) const { return this->findSubGroup(_subGroupName) != nullptr; };

		//! @brief Creates and adds a default tool button.
		//! @param _iconPath Button icon path that will be used in the icon manager to load the icon.
		//! @param _text Button text.
		//! @param _menu Menu to set.
		//! @return Created ToolButton.
		//! @ref ToolButton
		ToolButton* addToolButton(const QString& _iconPath, const QString& _text, QMenu* _menu = (QMenu*)nullptr);

		//! @brief Creates and adds a default tool button.
		//! @param _icon Button icon.
		//! @param _text Button text.
		//! @param _menu Menu to set.
		//! @return Created ToolButton.
		//! @ref ToolButton
		ToolButton* addToolButton(const QIcon& _icon, const QString& _text, QMenu* _menu = (QMenu*)nullptr);

	private:
		std::string m_name;
		tt::Group* m_group;

		TabToolBarPage* m_parentPage;
		std::list<TabToolBarSubGroup*> m_subGroups;
	};

}