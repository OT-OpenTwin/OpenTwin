/*
 *	File:		aOptionsDialog.cpp
 *	Package:	akDialogs
 *
 *  Created on: August 26, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// uiCore header
#include <akCore/globalDataTypes.h>
#include <akGui/aDialog.h>
#include <akGui/aColor.h>
#include <akWidgets/aWidget.h>

// Qt header
#include <qstring.h>
#include <qstringlist.h>

// C++ header
#include <vector>

class QWidget;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QSplitter;
class QScrollArea;
class QListWidgetItem;

namespace ak {

	class aLabelWidget;
	class aCheckBoxWidget;
	class aComboButtonWidget;
	class aComboBoxWidget;
	class aColorEditButtonWidget;
	class aListWidget;
	class aLineEditWidget;
	class aTreeWidget;
	class aPushButtonWidget;
	class aSpinBoxWidget;
	class aDoubleSpinBoxWidget;

	class aOptionsGroup;
	class aAbstractOptionsItem;
	class aBasicOptionsItem;
	
	class UICORE_API_EXPORT aOptionsDialog : public aDialog {
		Q_OBJECT
	public:
		enum optionsDialogStyle {
			optionsApplySaveCancel,
			optionsClose
		};

		aOptionsDialog(optionsDialogStyle _style, QWidget * _parentWidget = nullptr);
		virtual ~aOptionsDialog();

		// #############################################################################################################

		// Base class functions

		virtual dialogResult showDialog(void) override;

		virtual dialogResult showDialog(QWidget * _parentWidget) override;

		dialogResult showDialog(aOptionsGroup * _groupToShow, QWidget * _parentWidget);

		virtual void closeEvent(QCloseEvent * _event) override;

		// #############################################################################################################

		// Setter

		aOptionsGroup * addGroup(const QString& _name, const QString& _title);

		void addGroup(aOptionsGroup * _group);

		void removeGroup(const QString& _name);

		void removeGroup(aOptionsGroup * _group);

		void addWidget(QWidget * _w);

		void removeWidget(QWidget * _w);

		void setNavigationSearchPlaceholderText(const QString& _text);

		void selectGroupByLogicalName(const QString& _logicalName, const QString& _delimiter = QString(":"));

		// #############################################################################################################

		// Getter

		aTreeWidget * navigation(void) { return m_navigation; }

		const std::vector<aOptionsGroup *>& groups(void) const { return m_groups; }

		const QString& lastSelectedGroupName(void) { return m_lastGroupName; }

		std::list<aAbstractOptionsItem *> items(void) const;

		aOptionsGroup * findGroupByNavigationId(ID _id);

		aOptionsGroup * findGroupByLogicalName(const QString& _logicalName, const QString& _delimiter = QString(":"));

	Q_SIGNALS:
		void saveSettingsRequested(void);
		void itemChanged(aAbstractOptionsItem * item);

	private Q_SLOTS:
		void slotShowGroup();
		void slotRemoveGroup(aOptionsGroup * _group);
		void slotNavigationChanged(void);
		void slotSave(void);
		void slotCancel(void);
		void slotApply(void);
		void slotEmitChanged(aAbstractOptionsItem * _item);
	private:

		friend class aOptionsGroup;
		void clearSettingsArea(void);
		void groupItemChanged(aAbstractOptionsItem * _item);

		bool							m_itemChangedIsQueued;
		int								m_ignoreEvents;
		bool							m_isClosing;
		optionsDialogStyle				m_style;
		std::vector<aOptionsGroup *>	m_groups;
		aOptionsGroup *					m_activeGroup;
		QString							m_lastGroupName;
		aOptionsGroup *					m_requestedGroup;

		QVBoxLayout *					m_centralLayout;
		
		QWidget *						m_buttonLayoutW;
		QHBoxLayout *					m_buttonLayout;

		QWidget *						m_dummy;

		QSplitter *						m_splitter;

		aTreeWidget *					m_navigation;
		QScrollArea *					m_settingsArea;

		QWidget *						m_settingsLayoutW;
		QVBoxLayout *					m_settingsLayout;

		QWidget *						m_settingsAreaLayoutW;
		QVBoxLayout *					m_settingsAreaLayout;

		aPushButtonWidget *				m_buttonSave;
		aPushButtonWidget *				m_buttonCancel;
		aPushButtonWidget *				m_buttonApply;

		aOptionsDialog() = delete;
		aOptionsDialog(aOptionsDialog&) = delete;
		aOptionsDialog& operator = (aOptionsDialog&) = delete;

	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsGroup : public aObject {
	public:
		explicit aOptionsGroup(const QString& _name, const QString& _title, aOptionsGroup * _parentGroup = nullptr);
		virtual ~aOptionsGroup();
				
		// #############################################################################################################

		// View manipulation

		void show(aOptionsDialog * _dialog);

		void hide(aOptionsDialog * _dialog);

		// #############################################################################################################

		// Setter

		aOptionsGroup * addGroup(const QString& _name, const QString& _title);

		void addGroup(aOptionsGroup * _group);
		
		void addItem(aAbstractOptionsItem * _item);

		void attachToDialog(aOptionsDialog * _dialog);

		void attachToGroup(aOptionsGroup * _group);

		void displayNavigationItem(void);

		void hideNavigationItem(void);

		// #############################################################################################################

		// Getter

		const QString& title(void) const { return m_title; }

		const QString& name(void) const { return m_name; }

		ID navigationID(void) const { return m_navigationID; }

		bool isVisible(void) const { return m_isVisible; }

		const std::vector<aOptionsGroup *>& subgroups(void) const { return m_groups; }
		const std::vector<aAbstractOptionsItem *>& items(void) const { return m_items; }

		const aOptionsGroup * parentGroup(void) const { return m_parentGroup; }
		aOptionsGroup * parentGroup(void) { return m_parentGroup; }

		aOptionsGroup * findGroupByNavigationId(ID _id);

		bool hasSubgroup(aOptionsGroup * _group);

		bool hasDialog(void);

		aOptionsDialog * getDialog(void);

		QString logicalName(const QString& _delimiter = ":");

		aOptionsGroup * findSubgroupByLogicalName(const QString& _logicalName, const QString& _delimiter = QString(":"));
		aOptionsGroup * findSubgroupByLogicalName(const QStringList& _logicalName, int _index = 0);

		aAbstractOptionsItem * findItemByLogicalName(const QString& _name, const QString& _delimiter = QString(":"));
		aAbstractOptionsItem * findItemByLogicalName(const QStringList& _logicalName, int _index = 0);

		void itemChanged(aAbstractOptionsItem * _item);
		
	private:

		aOptionsDialog *					m_dialog;
		QString								m_name;
		QString								m_title;
		bool								m_isVisible;
		ID									m_navigationID;

		QWidget *							m_titleLayoutW;
		QHBoxLayout *						m_titleLayout;
		aLabelWidget *						m_titleLabel;

		aOptionsGroup *						m_parentGroup;

		std::vector<aOptionsGroup *>		m_groups;
		std::vector<aAbstractOptionsItem *>	m_items;
	
		aOptionsGroup() = delete;
		aOptionsGroup(aOptionsGroup&) = delete;
		aOptionsGroup& operator = (aOptionsGroup&) = delete;

	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aAbstractOptionsItem : public QObject, public aWidget {
		Q_OBJECT
	public:
		enum optionItemType {
			InfoText,
			LineEdit,
			List,
			SpinBox,
			DoubleSpinBox,
			ColorSelect,
			CheckBox,
			ComboBox,
			ComboButton,
			DirectorySelect,
			FileSelectSave,
			FileSelectOpen,
			Custom
		};

		aAbstractOptionsItem(optionItemType _type, const QString& _name, const QString& _title);
		virtual ~aAbstractOptionsItem();

		// #############################################################################################################

		// Base class functions

		virtual QWidget * widget(void) override = 0;

		// #############################################################################################################

		// View manipulation

		void constructWidget(void);

		void Show(aOptionsDialog * _dialog);

		void Hide(aOptionsDialog * _dialog);

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) = 0;

		void attachToGroup(aOptionsGroup * _group);

		void displayNavigationItem(void);

		void hideNavigationItem(void);

		// #############################################################################################################

		// Getter

		const QString& name(void) const { return m_name; }

		const QString& title(void) const { return m_title; }

		const aOptionsGroup * parentGroup(void) const { return m_parentGroup; }

		aOptionsGroup * parentGroup(void) { return m_parentGroup; }

		optionItemType type(void) const { return m_type; }

		QString logicalName(const QString& _delimiter = QString(":")) const;
		
		// #############################################################################################################

	protected:

		virtual void ConstructWidget(void) = 0;
		
		void emitChanged(void);

		optionItemType		m_type;
		aOptionsGroup *		m_parentGroup;
		
	private:

		ID					m_navigationId;
		QString				m_name;
		QString				m_title;
		bool				m_isVisible;
		bool				m_widgetIsConstructed;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aBasicOptionsItem : public aAbstractOptionsItem {
		Q_OBJECT
	public:

		aBasicOptionsItem(optionItemType _type, const QString& _name, const QString& _title = QString());
		virtual ~aBasicOptionsItem();

		// #############################################################################################################

		// Base class functions

		virtual QWidget * widget(void) override { return m_centralLayoutW; }

		// #############################################################################################################

		// Setter
		
		virtual void setExplanation(const QString& _explanation) override;

		void setLabelVisible(bool _isVisible);

		void setLabel(const QString& _text);

		// #############################################################################################################

		// Getter

		QString labelText(void) const;

	protected:

		virtual void ConstructWidget(void) override;

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) = 0;

		bool							m_supressEvents;

	protected Q_SLOTS:
		void slotChanged(void);
		void slotChanged(int _v);
		void slotChanged(double _v);
		void slotChanged(const QString& _v);
		void slotChanged(QListWidgetItem * _v);

	private:
		friend class aOptionsItemInfoText;
		bool							m_showLabel;
		
		QWidget *						m_centralLayoutW;
		QHBoxLayout *					m_centralLayout;

		aLabelWidget *					m_label;

		aBasicOptionsItem() = delete;
		aBasicOptionsItem(aBasicOptionsItem&) = delete;
		aBasicOptionsItem& operator = (aBasicOptionsItem&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemInfoText : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemInfoText(const QString& _name, const QString& _title = QString());

		virtual ~aOptionsItemInfoText();

		// #############################################################################################################

		// Setter

		void setText(const QString& _text);

		// #############################################################################################################

		// Getter
		
		QString text(void) const;

		aLabelWidget * label(void);

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;

	private:

		aOptionsItemInfoText(aOptionsItemInfoText&) = delete;
		aOptionsItemInfoText& operator = (aOptionsItemInfoText&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemCheckBox : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemCheckBox(const QString& _name, const QString& _title = QString(), bool _isChecked = false);

		virtual ~aOptionsItemCheckBox();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setChecked(bool _isChecked = true);

		void setTristate(void);

		// #############################################################################################################

		// Getter

		Qt::CheckState checkState(void) const { return m_state; }

		bool isChecked(void) const { return m_isChecked; }

		aCheckBoxWidget * checkBox(void) { return m_checkbox; }

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;
		
	private Q_SLOTS:
		void slotCheckStateChanged(int _state);

	private:

		bool					m_isChecked;
		Qt::CheckState			m_state;
		aCheckBoxWidget *		m_checkbox;

		aOptionsItemCheckBox(aOptionsItemCheckBox&) = delete;
		aOptionsItemCheckBox& operator = (aOptionsItemCheckBox&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemLineEdit : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemLineEdit(const QString& _name, const QString& _title = QString(), const QString& _text = QString());

		virtual ~aOptionsItemLineEdit();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setText(const QString& _text);

		void setPlaceholderText(const QString& _placeholderText);
		
		// #############################################################################################################

		// Getter

		QString text(void) const;

		QString placeholderText(void) const;

		aLineEditWidget * lineEdit(void) { return m_lineEdit; }

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;
		
	private:

		aLineEditWidget *		m_lineEdit;

		aOptionsItemLineEdit(aOptionsItemLineEdit&) = delete;
		aOptionsItemLineEdit& operator = (aOptionsItemLineEdit&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemSpinBox : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemSpinBox(const QString& _name, const QString& _title = QString(), int _value = 0, int _min = 0, int _max = 100);

		virtual ~aOptionsItemSpinBox();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setValue(int _value);

		void setMinValue(int _minValue);

		void setMaxValue(int _maxValue);

		void setLimits(int _min, int _max);

		// #############################################################################################################

		// Getter

		int value(void) const;

		int minValue(void) const;

		int maxValue(void) const;

		aSpinBoxWidget * spinBox(void) { return m_spinBox; }

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;

	private:

		aSpinBoxWidget *		m_spinBox;

		aOptionsItemSpinBox(aOptionsItemSpinBox&) = delete;
		aOptionsItemSpinBox& operator = (aOptionsItemSpinBox&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemDoubleSpinBox : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemDoubleSpinBox(const QString& _name, const QString& _title = QString(), double _value = 0, double _min = 0, double _max = 100, int _decimals = 2);

		virtual ~aOptionsItemDoubleSpinBox();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setValue(double _value);

		void setMinValue(double _minValue);

		void setMaxValue(double _maxValue);

		void setLimits(double _min, double _max);

		void setDecimals(int _decimals);

		// #############################################################################################################

		// Getter

		double value(void) const;

		double minValue(void) const;

		double maxValue(void) const;

		int decimals(void) const;

		aDoubleSpinBoxWidget * spinBox(void) { return m_spinBox; }

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;

	private:

		aDoubleSpinBoxWidget *		m_spinBox;

		aOptionsItemDoubleSpinBox(aOptionsItemDoubleSpinBox&) = delete;
		aOptionsItemDoubleSpinBox& operator = (aOptionsItemDoubleSpinBox&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemComboBox : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemComboBox(const QString& _name, const QString& _title = QString(), const QStringList& _possibleSelection = QStringList(), const QString& _value = QString());

		virtual ~aOptionsItemComboBox();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setPossibleSelection(const QStringList& _possibleSelection);

		void setText(const QString& _text);

		// #############################################################################################################

		// Getter

		QStringList possibleSelection(void) const;

		QString text(void) const;

		aComboBoxWidget * comboBox(void) { return m_comboBox; }

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;
		
	private:

		aComboBoxWidget *		m_comboBox;

		aOptionsItemComboBox(aOptionsItemComboBox&) = delete;
		aOptionsItemComboBox& operator = (aOptionsItemComboBox&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemComboButton : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemComboButton(const QString& _name, const QString& _title = QString(), const QStringList& _possibleSelection = QStringList(), const QString& _value = QString());

		virtual ~aOptionsItemComboButton();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setPossibleSelection(const QStringList& _possibleSelection);

		void setText(const QString& _text);

		// #############################################################################################################

		// Getter

		QStringList possibleSelection(void) const;

		QString text(void) const;

		aComboButtonWidget * comboButton(void) { return m_comboButton; }

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;

	private:

		aComboButtonWidget *		m_comboButton;

		aOptionsItemComboButton(aOptionsItemComboButton&) = delete;
		aOptionsItemComboButton& operator = (aOptionsItemComboButton&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemColorEdit : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemColorEdit(const QString& _name, const QString& _title = QString(), const aColor& _color = aColor(0, 0, 0, 255));

		virtual ~aOptionsItemColorEdit();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setColor(const aColor& _color);

		void setEditAlphaChannel(bool _isActive);

		// #############################################################################################################

		// Getter

		aColor color(void) const;

		bool editAlphaChannel(void) const;

		aColorEditButtonWidget * colorEdit(void) { return m_colorEdit; }

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;

	private:

		aColorEditButtonWidget *		m_colorEdit;

		aOptionsItemColorEdit(aOptionsItemComboButton&) = delete;
		aOptionsItemColorEdit& operator = (aOptionsItemColorEdit&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemList : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemList(const QString& _name, const QString& _title = QString(), const QStringList& _possibleSelection = QStringList(), const QStringList& _currentSelection = QStringList());

		virtual ~aOptionsItemList();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setPossibleSelection(const QStringList& _possibleSelection);

		void setCurrentSelection(const QStringList& _selection);

		// #############################################################################################################

		// Getter

		QStringList possibleSelection(void) const;

		QStringList currentSelection(void) const;

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;

	private:

		aListWidget *		m_list;

		aOptionsItemList(aOptionsItemList&) = delete;
		aOptionsItemList& operator = (aOptionsItemList&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemDirectorySelect : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemDirectorySelect(const QString& _name, const QString& _title = QString(), const QString& _initialDirectory = QString());

		virtual ~aOptionsItemDirectorySelect();

		// #############################################################################################################

		// Setter
		
		virtual void setExplanation(const QString& _explanation) override;

		void setSelectedDirectory(const QString& _directory);

		// #############################################################################################################

		// Getter

		aLineEditWidget * lineEdit(void) { return m_edit; }

		aPushButtonWidget * button(void) { return m_button; }

		QString selectedDirectory(void) const;

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;

	private Q_SLOTS:
		void slotButtonClicked(void);

	private:

		aLineEditWidget *	m_edit;
		aPushButtonWidget *	m_button;

		aOptionsItemDirectorySelect(aOptionsItemDirectorySelect&) = delete;
		aOptionsItemDirectorySelect& operator = (aOptionsItemDirectorySelect&) = delete;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemFileSelectSave : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemFileSelectSave(const QString& _name, const QString& _title = QString(), const QString& _initialDirectory = QString(), const QString& _filter = QString());

		virtual ~aOptionsItemFileSelectSave();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setFilter(const QString& _filter) { m_filter = _filter; }

		void setSelectedFile(const QString& _directory);

		// #############################################################################################################

		// Getter

		aLineEditWidget * lineEdit(void) { return m_edit; }

		aPushButtonWidget * button(void) { return m_button; }

		QString filter(void) const { return m_filter; }

		QString selectedFile(void) const;

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;

	private Q_SLOTS:
		void slotButtonClicked(void);

	private:

		QString				m_filter;
		aLineEditWidget *	m_edit;
		aPushButtonWidget *	m_button;

		aOptionsItemFileSelectSave(aOptionsItemFileSelectSave&) = delete;
		aOptionsItemFileSelectSave& operator = (aOptionsItemFileSelectSave&) = delete;
	};
	
	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class UICORE_API_EXPORT aOptionsItemFileSelectOpen : public aBasicOptionsItem {
		Q_OBJECT
	public:
		aOptionsItemFileSelectOpen(const QString& _name, const QString& _title = QString(), const QString& _initialDirectory = QString(), const QString& _filter = QString());

		virtual ~aOptionsItemFileSelectOpen();

		// #############################################################################################################

		// Setter

		virtual void setExplanation(const QString& _explanation) override;

		void setFilter(const QString& _filter) { m_filter = _filter; }

		void setSelectedFile(const QString& _directory);

		// #############################################################################################################

		// Getter

		aLineEditWidget * lineEdit(void) { return m_edit; }

		aPushButtonWidget * button(void) { return m_button; }

		QString filter(void) const { return m_filter; }

		QString selectedFile(void) const;

	protected:

		virtual void addItemWidgetsToLayout(QHBoxLayout * _layout) override;

	private Q_SLOTS:
		void slotButtonClicked(void);

	private:

		QString				m_filter;
		aLineEditWidget *	m_edit;
		aPushButtonWidget *	m_button;

		aOptionsItemFileSelectOpen(aOptionsItemFileSelectOpen&) = delete;
		aOptionsItemFileSelectOpen& operator = (aOptionsItemFileSelectOpen&) = delete;
	};

	Q_DECLARE_METATYPE(aOptionsGroup*);
	Q_DECLARE_METATYPE(aAbstractOptionsItem*);

}
