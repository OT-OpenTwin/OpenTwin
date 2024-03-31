/*
 *	File:		aPropertyGridWidget.h
 *	Package:	akWidgets
 *
 *  Created on: August 2, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// uiCore header
#include <akWidgets/aWidget.h>

#include <akGui/aColor.h>

#include <akCore/aTime.h>
#include <akCore/aDate.h>

// Qt header
#include <qobject.h>
#include <qicon.h>
#include <qstring.h>
#include <qstringlist.h>

// C++ header

// Forward declarations
class QWidget;
class QVBoxLayout;
class QTableWidgetItem;

namespace ak {

	class aTableWidget;
	class aLabelWidget;
	class aTimePickWidget;
	class aDatePickWidget;
	class aComboButtonWidget;
	class aCheckBoxWidget;
	class aPushButtonWidget;
	class aColorEditButtonWidget;
	class aSpinBoxWidget;

	class aPropertyGridGroup;
	class aPropertyGridItem;

	class UICORE_API_EXPORT aPropertyGridWidget : public QObject, public aWidget {
		Q_OBJECT
	public:
		aPropertyGridWidget(QWidget * _parent = (QWidget *) nullptr);
		virtual ~aPropertyGridWidget();

		// ##############################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override { return m_centralLayoutW; }

		// ##############################################################################################

		// Item creation

		void addGroup(const QString& _groupName, const aColor& _groupColor = aColor(255, 255, 255), const aColor& _foregroundColor = aColor(0, 0, 0), const aColor& _errorColor = aColor(255, 0, 0));

		ID addItem(const QString& _groupName, const QString& _itemName, bool _value, bool _isMultipleValues = false);
		ID addItem(const QString& _groupName, const QString& _itemName, int _value, bool _isMultipleValues = false);
		ID addItem(const QString& _groupName, const QString& _itemName, int _value, int _min, int _max, bool _isMultipleValues = false);
		ID addItem(const QString& _groupName, const QString& _itemName, double _value, bool _isMultipleValues = false);
		ID addItem(const QString& _groupName, const QString& _itemName, const QString& _value, bool _isMultipleValues = false);
		ID addItem(const QString& _groupName, const QString& _itemName, const aTime& _value, timeFormat _timeFormat, bool _isMultipleValues = false);
		ID addItem(const QString& _groupName, const QString& _itemName, const aDate& _value, dateFormat _dateFormat, bool _isMultipleValues = false);
		ID addItem(const QString& _groupName, const QString& _itemName, const aColor& _value, bool _isMultipleValues = false);
		ID addItem(const QString& _groupName, const QString& _itemName, const QStringList& _possibleSelection, const QString& _value, bool _isMultipleValues = false);

		// ##############################################################################################

		// Manipulator

		void setEnabled(bool _isEnabled);

		void setGroupIcons(const QIcon& _expand, const QIcon& _collapse);
		void setDeleteIcon(const QIcon& _delete);

		void clear(bool _keepGroups = false);

		// ##############################################################################################

		// Getter

		aPropertyGridGroup * group(const QString& _groupName);

		aPropertyGridItem * item(ID _id);

		aPropertyGridItem * findItem(const QString& _itemName);

		QIcon& getDeleteIcon(void) { return m_iconDelete; }

		bool isEnabled(void) const { return m_isEnabled; }

	Q_SIGNALS:
		void itemChanged(ID _itemId);
		void cleared(void);
		void itemDeleted(ID _itemId);

	private Q_SLOTS:
		void slotItemChanged(void);
		void slotFocusOut(void);
		void slotItemDeleted(void);

	private:

		friend class aPropertyGridGroup;

		ak::ID newItemCreated(aPropertyGridGroup * _group, aPropertyGridItem * _item);
		void eraseItemFromMap(aPropertyGridItem * _item);
		void updateTableVisibility(void);

		QWidget *									m_centralLayoutW;
		QVBoxLayout *								m_centralLayout;

		aTableWidget *								m_table;
		aLabelWidget *								m_label;
		QWidget *									m_stretch;

		aPropertyGridGroup *						m_defaultGroup;

		ID											m_currentID;
		std::map<QString, aPropertyGridGroup *>		m_groups;
		std::map<ID, aPropertyGridItem *>			m_items;

		QIcon										m_iconGroupExpand;
		QIcon										m_iconGroupCollapse;
		QIcon										m_iconDelete;
		bool										m_groupIconsSet;

		bool										m_isEnabled;

		aPropertyGridWidget(aPropertyGridWidget&) = delete;
		aPropertyGridWidget& operator = (aPropertyGridWidget&) = delete;
	};

	// #############################################################################################################################################

	// #############################################################################################################################################

	// #############################################################################################################################################

	class UICORE_API_EXPORT aPropertyGridGroup : public QObject {
		Q_OBJECT
	public:
		aPropertyGridGroup(aPropertyGridWidget * _owner, const QString& _title, const aColor& _color, const aColor& _foregroundColor, const aColor& _errorColor);

		virtual ~aPropertyGridGroup();

		// ##############################################################################################

		// Getter

		const QString& title(void) const { return m_title; }

		int lastItemRow(void) const;

		int headerRow(void) const;

		aPropertyGridWidget* getOwner(void) const { return m_owner; }
		
		// ##############################################################################################

		// Manipulators

		void repaint(void);

		//! @brief Will add the provided item to this group
		//! This will also set the items colors 
		void addItem(aPropertyGridItem * _item);

		//! @brief Will set the group colors and calculate the alternate color
		//! @param _color The regular background color
		//! @param _foregroundColor The regular foreground color
		//! @param _errorColor The error foreground color
		void setGroupColors(const aColor& _color, const aColor& _foregroundColor, const aColor& _errorColor, bool _repaintItems = true);
		
		//! @brief Will apply the group state icons
		void refreshStateIcons(void);

		//! @brief Will clear all items from this group
		void clear(void);

		//! @brief Will clear the groups focus (reset the items to not focused)
		void clearFocus(void);

	private Q_SLOTS:
		void slotItemDoubleClicked(QTableWidgetItem * _item);

	private:
		aPropertyGridWidget *				m_owner;
		QTableWidgetItem *					m_headerItem;
		QString								m_title;

		aColor								m_color;
		aColor								m_colorAlternate;
		aColor								m_colorForeground;
		aColor								m_colorForegroundError;
		aColor								m_headerColorB;
		aColor								m_headerColorF;

		bool								m_isAlternate;
		bool								m_isCollapsed;

		std::vector<aPropertyGridItem *>	m_items;

		aPropertyGridGroup() = delete;
		aPropertyGridGroup(aPropertyGridGroup&) = delete;
		aPropertyGridGroup& operator = (aPropertyGridGroup&) = delete;
	};

	// #############################################################################################################################################

	// #############################################################################################################################################

	// #############################################################################################################################################

	class UICORE_API_EXPORT aPropertyGridItem : public QObject {
		Q_OBJECT
	public:
		enum NumberInputMode {
			TextForm,
			NumericUpDown
		};

		explicit aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, bool _value, bool _isMultipleValues = false);
		explicit aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, int _value, NumberInputMode _inputMode, int _min, int _max, bool _isMultipleValues = false);
		explicit aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, double _value, bool _isMultipleValues = false);
		explicit aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const QString& _value, bool _isMultipleValues = false);
		explicit aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const aTime& _value, timeFormat _timeFormat, bool _isMultipleValues = false);
		explicit aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const aDate& _value, dateFormat _dateFormat, bool _isMultipleValues = false);
		explicit aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const aColor& _value, bool _isMultipleValues = false);
		explicit aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const QStringList& _possibleSelection, const QString& _value, bool _isMultipleValues = false);

		virtual ~aPropertyGridItem();

		// ##############################################################################################

		// Getter

		//! @brief Will return the item ID
		ID id(void) const { return m_id; }

		//! @brief Will return true if the item is enabled
		bool isEnabled(void) const { return m_isEnabled; }

		//! @brief Will return true if the item is read-only
		bool isReadOnly(void) const { return m_isReadOnly; }

		//! @brief Will return true if the item is deletable
		bool isDeletable(void) const { return m_isDeletable; }

		//! @brief Will return the row this item is located at in the table
		int row(void) const;

		//! @brief Will return the item value type
		valueType getValueType(void) const { return m_valueType; }

		//! @brief Will return the item name
		const QString& name(void) const { return m_name; }

		//! @brief Will return the item group
		aPropertyGridGroup * group(void) const { return m_group; }

		//! @brief Will return true if the item is in a multiple values state
		bool isMultipleValues(void) const { return m_isMultipleValues; }

		// ##############################################################################################

		// Value setter/getter

		//! @brief Will return the items boolean value
		bool getBool(void) const { return m_vBool; }

		//! @brief Will return the items integer value
		int getInt(void) const { return m_vInt; }

		//! @brief Will return the items double value
		double getDouble(void) const { return m_vDouble; }

		//! @brief Will return the items string value
		const QString& getString(void) const { return m_vString; }

		//! @brief Will return the items selection value
		const QString& getSelection(void) const { return m_vSelection; }

		//! @brief Will return the items selection value
		const QStringList& getPossibleSelection(void) const { return m_vSelectionPossible; }

		//! @brief Will return the items color value
		const aColor& getColor(void) const { return m_vColor; }

		//! @brief Will return the items date value
		const aDate& getDate(void) const { return m_vDate; }

		//! @brief Will return the items time value
		const aTime& getTime(void) const { return m_vTime; }

		//! @brief Will set the items boolean value
		//! @param _value The value to set
		void setBool(bool _value);

		//! @brief Will set the items integer value
		//! @param _value The value to set
		void setInt(int _value);

		//! @brief Will set the items double value
		//! @param _value The value to set
		void setDouble(double _value);

		//! @brief Will set the items string/selection value
		//! @param _value The value to set
		void setString(const QString& _value);

		//! @brief Will set the items color value
		//! @param _value The value to set
		void setColor(const aColor& _value);

		//! @brief Will set the items time value
		//! @param _value The value to set
		void setTime(const aTime& _value);

		//! @brief Will set the items date value
		//! @param _value The value to set
		void setDate(const aDate& _value);

		// ##############################################################################################

		// Manipulators

		//! @brief Will refresh the color settings
		//! This function has the same effect as calling setErrorState with _forceRepaint = true
		void repaint(void);

		//! @brief Will set the enabled state of the item
		//! @param _isEnabled The enabled state to set
		void setEnabled(bool _isEnabled = true);

		//! @brief Will set the read-only state of the item
		//! @param _isReadOnly The read-only state to set
		void setReadOnly(bool _isReadOnly = true);

		//! @brief Will set the deletable state of the item
		//! @param _isDeletable The deletable state to set
		void setDeletable(bool _isDeletable);

		//! @brief Will set the item ID
		//! @param _id The ID to set
		void setId(ID _id) { m_id = _id; }

		//! @brief Will set the item colors
		//! @param _normalForeground The normal foreground color
		//! @param _normalBackground The normal background color
		//! @param _errorForeground The error foreground color
		void setColor(const aColor& _normalForeground, const aColor& _normalBackground, const aColor& _errorForeground);

		//! @brief Will set the group this item is located at
		//! @param _group The group to set
		void setGroup(aPropertyGridGroup * _group) { m_group = _group; }

		void clearFocus(void);

		// ##############################################################################################

		// Error

		//! @brief Will set the error state for this item
		//! @param _isError If true the item will be displayed in the error color
		void setErrorState(bool _isError, bool _forceRepaint = false);

		//! @brief Will set the items boolean and set the error state to true
		//! @param _value The value to set
		void setError(bool _value);

		//! @brief Will set the items integer and set the error state to true
		//! @param _value The value to set
		void setError(int _value);

		//! @brief Will set the items double and set the error state to true
		//! @param _value The value to set
		void setError(double _value);

		//! @brief Will set the items string/selection and set the error state to true
		//! @param _value The value to set
		void setError(const QString& _value);

		//! @brief Will set the items color and set the error state to true
		//! @param _value The value to set
		void setError(const aColor& _value);

		//! @brief Will set the items time and set the error state to true
		//! @param _value The value to set
		void setError(const aTime& _value);

		//! @brief Will set the items date and set the error state to true
		//! @param _value The value to set
		void setError(const aDate& _value);

	Q_SIGNALS:
		void changed(void);
		void deleted(void);

	private Q_SLOTS:

		void slotValueChanged(void);

		void slotValueBoolChanged(int _v);

		void slotItemChanged(QTableWidgetItem * _item);

		void slotItemDeleted(void);

	private:

		void ini(void);

		void updateEditability(void);

		ID							m_id;
		NumberInputMode				m_numberInputMode;
		aPropertyGridGroup *		m_group;
		bool						m_isReadOnly;
		bool						m_isDeletable;
		bool						m_isEnabled;
		bool						m_ignoreEvent;
		bool						m_isError;
		bool						m_isEditable;
		valueType					m_valueType;

		aColor						m_colorNormal;
		aColor						m_colorError;
		aColor						m_colorBackground;
		
		// Controls

		aTableWidget *				m_table;
		int							m_row;
		QTableWidgetItem *			m_nameItem;

		QTableWidgetItem *			m_textItem;
		aCheckBoxWidget *			m_cBool;
		aColorEditButtonWidget *	m_cColor;
		aComboButtonWidget *		m_cSelection;
		aTimePickWidget *			m_cTime;
		aDatePickWidget *			m_cDate;
		aSpinBoxWidget *			m_cInt;
		aPushButtonWidget*          m_cDelete;


		// Values

		QString						m_name;
		bool						m_isMultipleValues;
		bool						m_vBool;
		int							m_vInt;
		int							m_vIntMin;
		int							m_vIntMax;
		double						m_vDouble;
		QString						m_vString;
		QString						m_vSelection;
		QStringList					m_vSelectionPossible;
		aTime						m_vTime;
		timeFormat					m_vTimeFormat;
		aDate						m_vDate;
		dateFormat					m_vDateFormat;
		aColor						m_vColor;

		aPropertyGridItem() = delete;
		aPropertyGridItem(aPropertyGridItem&) = delete;
		aPropertyGridItem& operator = (aPropertyGridItem&) = delete;
	};

}
