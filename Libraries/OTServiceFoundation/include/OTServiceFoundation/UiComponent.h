/*
 *  UiComponent.h
 *
 *  Created on: 10/02/2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021, OpenTwin
 */

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/ServiceBase.h"			// Base class
#include "OTGui/GuiTypes.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/StyledTextBuilder.h"
#include "OTServiceFoundation/FoundationAPIExport.h"
#include "OTServiceFoundation/MenuButtonDescription.h"
#include "OTServiceFoundation/ContextMenu.h"

// C++ header
#include <list>
#include <map>
#include <string>

namespace ot {
	
	class ApplicationBase;
	
	namespace components {

		class UiPluginComponent;

		class OT_SERVICEFOUNDATION_API_EXPORT UiComponent : public ServiceBase {
		public:

			enum entitySelectionType {
				FACE,
				EDGE,
				SHAPE,
				TRANSFORM
			};

			enum entitySelectionAction {
				PORT,
				REMOVE_FACE,
				BOOLEAN_ADD,
				BOOLEAN_SUBTRACT,
				BOOLEAN_INTERSECT,
				TRANSFORM_SHAPES,
				CHAMFER_EDGE,
				BLEND_EDGE
			};

			UiComponent(
				const std::string &			_name,
				const std::string &			_type,
				const std::string &			_url,
				serviceID_t					_id,
				ApplicationBase *			_application
			);

			virtual ~UiComponent();

			// #####################################################################################################################

			// Element creation

			//! @brief Will add a new menu page in the UI
			//! @param _pageName The name of the page to add
			void addMenuPage(
				const std::string &			_pageName
			);

			//! @brief Will add a new menu group in the specified menu page at the UI
			//! @param _pageName The name of the page to add the group at
			//! @param _groupName The name of the group
			void addMenuGroup(
				const std::string &			_pageName,
				const std::string &			_groupName
			);

			//! @brief Will add a new menu subgroup in the specified menu group at the UI
			//! @param _pageName The name of the page to add the group at
			//! @param _groupName The name of the group
			void addMenuSubGroup(
				const std::string &			_pageName,
				const std::string &			_groupName,
				const std::string &			_subgroupName
			);
			
			//! @brief Will add a new ToolButton in the specified menu group at the UI
			//! @param _pageName The name of the page the group is located at
			//! @param _groupName The name of the group to add the action at
			//! @param _buttonName The name of the button
			//! @param _text The text of the button
			//! @param _keySequence The shortcut key sequence to trigger this button (you can use ot::ui::keySequenceToString() for help, include openTwin/uiTypes)
			//! @param _lockTypes The lock type groups that this control belongs to (refer to documentation)
			//! @param _iconName The name of the icon that should be displayed in the action (refer to documentation)
			//! @param _iconFolder The folder the icon is located at (refer to documentation)
			void addMenuButton(
				const std::string &			_pageName,
				const std::string &			_groupName,
				const std::string &			_buttonName,
				const std::string &			_text,
				const LockTypeFlags &		_lockTypes,
				const std::string &			_iconName,
				const std::string &			_iconFolder = std::string("Default"),
				const std::string &			_keySequence = std::string(""),
				const ContextMenu&			_contextMenu = ContextMenu("")
			);

			void addMenuButton(
				ot::MenuButtonDescription&	_menuButtonDescription,
				const LockTypeFlags&		_lockTypes,
				const std::string &			_iconName,
				const std::string &			_iconFolder = std::string("Default"),
				const std::string &			_keySequence = std::string(""),
				const ContextMenu&			_contextMenu = ContextMenu("")
			);


			//! @brief Will add a new ToolButton in the specified menu sub group at the UI
			//! @param _pageName The name of the page the group is located at
			//! @param _groupName The name of the group the subgroup is located at
			//! @param _subgroupName The name of the subgorup the button will be placed at
			//! @param _buttonName The name of the button
			//! @param _text The text of the button
			//! @param _lockTypes The lock type groups that this control belongs to (refer to documentation)
			//! @param _iconName The name of the icon that should be displayed in the action (refer to documentation)
			//! @param _iconFolder The folder the icon is located at (refer to documentation)
			//! @param _keySequence The shortcut key sequence to trigger this button (you can use ot::ui::keySequenceToString() for help, include openTwin/uiTypes)
			void addMenuButton(
				const std::string &			_pageName,
				const std::string &			_groupName,
				const std::string &			_subgroupName,
				const std::string &			_buttonName,
				const std::string &			_text,
				const LockTypeFlags&		_lockTypes,
				const std::string &			_iconName,
				const std::string &			_iconFolder = std::string("Default"),
				const std::string &			_keySequence = std::string(""),
				const ContextMenu&			_contextMenu = ContextMenu("")
			);

			//! @brief Will add a new CheckBox in the specified menu group at the UI
			//! @param _pageName The name of the page the group is located at
			//! @param _groupName The name of the group to add the CheckBox at
			//! @param _buttonName The name of the CheckBox
			//! @param _iconName The name of the icon that should be displayed in the action (refer to documentation)
			//! @param _lockTypes The lock type groups that this control belongs to (refer to documentation)
			void addMenuCheckbox(
				const std::string &			_pageName,
				const std::string &			_groupName,
				const std::string &			_checkboxName,
				const std::string &			_checkboxText,
				bool						_isChecked,
				const LockTypeFlags&		_lockTypes
			);

			//! @brief Will add a new CheckBox in the specified menu sub group at the UI
			//! @param _pageName The name of the page the group is located at
			//! @param _groupName The name of the group the subgroup is located at
			//! @param _subgroupName The name of the subgorup the CheckBox will be placed at
			//! @param _buttonName The name of the CheckBox
			//! @param _iconName The name of the icon that should be displayed in the action (refer to documentation)
			//! @param _lockTypes The lock type groups that this control belongs to (refer to documentation)
			void addMenuCheckbox(
				const std::string &			_pageName,
				const std::string &			_groupName,
				const std::string &			_subgroupName,
				const std::string &			_checkboxName,
				const std::string &			_checkboxText,
				bool						_isChecked,
				const LockTypeFlags&		_lockTypes
			);

			//! @brief Will add a new LineEdit in the specified menu group at the UI
			//! @param _pageName The name of the page the group is located at
			//! @param _groupName The name of the group to add the LineEdit at
			//! @param _buttonName The name of the LineEdit
			//! @param _iconName The name of the icon that should be displayed in the action (refer to documentation)
			//! @param _lockTypes The lock type groups that this control belongs to (refer to documentation)
			void addMenuLineEdit(
				const std::string &			_pageName,
				const std::string &			_groupName,
				const std::string &			_lineEditName,
				const std::string &			_labelText,
				const std::string &			_initialText,
				const LockTypeFlags&	_lockTypes
			);

			//! @brief Will add a new LineEdit in the specified menu group at the UI
			//! @param _pageName The name of the page the group is located at
			//! @param _groupName The name of the group the subgroup is located at
			//! @param _subgroupName The name of the subgorup the LineEdit will be placed at
			//! @param _buttonName The name of the LineEdit
			//! @param _iconName The name of the icon that should be displayed in the action (refer to documentation)
			//! @param _lockTypes The lock type groups that this control belongs to (refer to documentation)
			void addMenuLineEdit(
				const std::string &			_pageName,
				const std::string &			_groupName,
				const std::string &			_subgroupName,
				const std::string &			_lineEditName,
				const std::string &			_labelText,
				const std::string &			_initialText,
				const LockTypeFlags&	_lockTypes
			);

			void activateMenuPage(
				const std::string &			_pageName
			);

			//! @brief Will add a new shortcut
			//! The shortcut will be listened application wide
			//! @param _keySequence The shortcut key sequence to trigger this button (you can use ot::ui::keySequenceToString() for help, include openTwin/uiTypes)
			void addShortcut(
				const std::string &			_keySequence
			);

			//! @brief Will create a rubberband and start the sequence according to the configuration
			//! @param _note The note for the rubberband, the note will be attatched to the response once the rubberband sequence is over
			//! @param _json The json document describing the rubberband
			void createRubberband(
				const std::string &			_note,
				const std::string &			_configurationJson
			);
			
			// #####################################################################################################################

			// Control manipulation

			//! @brief Will set the tool tip for the specified object that is placed in the menu
			//! @param _controlName The name of the control to set the tool tip at
			//! @param _toolTipText The text of the tool tip
			void setControlToolTip(
				const std::string &			_controlName,
				const std::string &			_toolTipText
			) const;

			//! @brief Will set the tool tip for the specified object that is placed in the menu
			//! @param _pageName The name of the page the controls' group is located at
			//! @param _groupName The name of the group the control is located at
			//! @param _controlName The name of the control to set the tool tip at
			//! @param _toolTipText The text of the tool tip
			void setControlToolTip(
				const std::string &			_pageName,
				const std::string &			_groupName,
				const std::string &			_controlName,
				const std::string &			_toolTipText
			) const;

			//! @brief Will set the tool tip for the specified object that is placed in the menu
			//! @param _pageName The name of the page the controls' group is located at
			//! @param _groupName The name of the group the controls' subgroup is located at
			//! @param _subgroupName The name of the subgroup the control is located at
			//! @param _controlName The name of the control to set the tool tip at
			//! @param _toolTipText The text of the tool tip
			void setControlToolTip(
				const std::string &				_gepaName,
				const std::string &				_groupName,
				const std::string &				_subgroupName,
				const std::string &				_controlName,
				const std::string &				_toolTipText
			) const;

			//! @brief Will set the enabled state of the provided controls
			//! @param _enabledControls The names of the controls that should be enabled
			//! @param _disabledControls The names of the controls that should be disabled
			void setControlsEnabledState(
				const std::list<std::string> &	_enabledControls,
				const std::list<std::string> &	_disabledControls
			) const;

			//! @brief Will set the provided values to the line edit
			//! @param _controlName The name of the line edit
			//! @param _text The text to set
			//! @param _errorState The error state to set
			void setLineEditValues(
				const std::string &				_controlName,
				const std::string &				_text,
				bool							_errorState = false
			) const;

			//! @brief Will set tthe provided values to the checkbox
			//! @param _controlName The name of the checkbox
			//! @param _checkedState The checked state to set
			void setCheckboxValues(
				const std::string &				_controlName,
				bool							_checkedState
			) const;

			//! @brief Will lock the UI Elements that belong to the provided lock type
			//! @param _lockTypes The lock type Groups to lock in the UI
			void lockUI(
				const LockTypeFlags&		_lockTypes
			) const;

			//! @brief Will unlock the UI Elements that belong to the provided lock type
			//! @param _lockTypes The lock type Groups to unlock in the UI
			void unlockUI(
				const LockTypeFlags&		_lockTypes
			) const;

			//! @brief Will set the progress bar text. This also turns on the progress bar visibility.
			//! @param message The text for the progress bar
			//! @param continuous Indicate whether the progress bar should show a continous animation (no percentages)
			void setProgressInformation(std::string message, bool continuous);

			//! @brief Will set the percentage of the progress bar
			//! @param percentage The percentage for the progress bar
			void setProgress(int percentage);

			//! @brief Will hide the progress bar
			void closeProgressInformation(void);

			//! @brief Will switch current menu tab to the one provided
			//! @param _pageName The name of the page
			void switchMenuTab(
				const std::string &				_pageName
			) const;

			//! @brief Will register for model events at the UI
			void registerForModelEvents(void) const;

			//! @brief Will deregister from the model events at the UI
			void deregisterForModelEvents(void) const;

			//! @brief Will add the provided path to the uiService icon manager
			//! This function has no effect when running in release mode
			void addIconSearchPath(const std::string& _iconPath);

			//! @brief Will notify the ui that this service completed its frontend setup
			void notifyUiSetupCompleted(void);

			// #####################################################################################################################

			// Information IO

			//! @brief Will display a message in the Frontends output window.
			//! @param _message The message to display.
			void displayMessage(const std::string& _message) const;

			//! \brief Will display a styled text message in the Frontends output window.
			//! \param _builder StyledTextBuilder containing the message
			void displayStyledMessage(const StyledTextBuilder& _builder) const;

			//! @brief Will display a message in the UIs debug output window
			//! @param _message The message to display
			void displayDebugMessage(
				const std::string &				_message
			) const;

			//! @brief Will display a message in a error prompt dialog
			//! @param _message The message to display
			void displayErrorPrompt(
				const std::string &				_message
			) const;

			//! @brief Will display a message in a warning prompt dialog
			//! @param _message The message to display
			void displayWarningPrompt(
				const std::string &				_message
			) const;

			//! @brief Will display a message in a information prompt dialog
			//! @param _message The message to display
			void displayInformationPrompt(
				const std::string &				_message
			) const;

			//! @brief Will add the control name of the item that is located at the menu
			//! @param _list The list to add the item to
			//! @param _pageName The name of the page the controls' group is located at
			//! @param _groupName The name of the group the controls' subgroup is located at
			//! @param _controlName The name of the control
			void addControlNameToList(
				std::list<std::string> &		_list,
				const std::string &				_pageName,
				const std::string &				_groupName,
				const std::string &				_controlName
			);

			//! @brief Will add the control name of the item that is located at the menu
			//! @param _list The list to add the item to
			//! @param _pageName The name of the page the controls' group is located at
			//! @param _groupName The name of the group the controls' subgroup is located at
			//! @param _subgroupName The name of the subgroup the control is located at
			//! @param _controlName The name of the control
			void addControlNameToList(
				std::list<std::string> &		_list,
				const std::string &				_pageName,
				const std::string &				_groupName,
				const std::string &				_subgroupName,
				const std::string &				_controlName
			);

			// #####################################################################################################################

			// Element deletion

			//! @brief Will remove the specified UI elements from the UI
			void removeUIElements(const std::list<std::string> & _itemList);

			//! @brief Will remove all UI elements this service has created
			void removeAllUIElements(void);

			// #####################################################################################################################

			// View manipulation

			void enterEntitySelectionMode(
				UID											_visualizationModelUID,
				entitySelectionType							_selectionType,
				bool										_allowMultipleSelection,
				const std::string &							_filter,
				entitySelectionAction						_actionType,
				const std::string &							_message,
				const std::map<std::string, std::string> &	_options,
				ot::serviceID_t								_serviceToReplyTo
			) const;

			// #####################################################################################################################

			void freeze3DView(UID _visualizationModelUID, bool _flag);
			void refreshSelection(UID _visualizationModelUID);
			void refreshAllViews(UID visualizationModelID);
			void sendUpdatedControlState(void);
			void setControlState(const std::string controlName, bool enabled);

			bool sendMessage(bool _queue, JsonDocument& _doc, std::string& _response);

			// Plugin

			//! @brief Will request the uiService to load the specified plugin
			//! The request will be queued.
			//! Upon connection the Service will be netified by providing a UiPluginComponent.
			//! @param _pluginName The name of the plugin. The name must be unique for a service and can be used to access the plugin later
			//! @param _filename The filename or the full path of the plugin
			void requestUiPlugin(const std::string& _pluginName, const std::string& _filename);

			//! @brief Will add the provided path to the uiService plugin manager
			//! If a plugin will be requested the provided path will be searched for the plugin.
			//! This function has no effect when running in release mode
			void addPluginSearchPath(const std::string& _pluginPath);

		protected:

			//! @brief Will return true if the provided control was created here
			//! Usually used in debug mode
			bool controlNameExists(const std::string & _name) const;
			ApplicationBase *									m_application;		//! The application base this component belings to
			std::list<std::string>								m_uiElements;		//! List of all UI elements that werde created by this component
			std::map<std::string, bool>							m_uiControlState;
			std::map<std::string, bool>							m_uiControlStateInUI;

		private:
			friend class ApplicationBase;
			void sendSettingsData(const PropertyGridCfg& _config);

			UiComponent(const UiComponent &) = delete;
			UiComponent & operator = (const UiComponent &) = delete;

		};

	}
}
