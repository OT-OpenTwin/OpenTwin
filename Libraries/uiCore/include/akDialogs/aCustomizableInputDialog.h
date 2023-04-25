/*
 *	File:		aCustomizableInputDialog.h
 *	Package:	akDialogs
 *
 *  Created on: July 09, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// uiCore header
#include <akCore/globalDataTypes.h>
#include <akCore/aDate.h>
#include <akCore/aTime.h>
#include <akGui/aDialog.h>

// C++ header
#include <vector>
#include <map>

class QWidget;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

namespace ak {
	class aWidget;
	class aPushButtonWidget;
	class aLabelWidget;
	class aLineEditWidget;
	class aTextEditWidget;
	class aColorStyle;

	class UICORE_API_EXPORT aCustomizableInputDialog : public aDialog {
		Q_OBJECT
	public:
		aCustomizableInputDialog(QWidget * _parent = (QWidget *) nullptr);
		virtual ~aCustomizableInputDialog();

		//! @brief Will set the current color style for this dialog and all controls
		//! @param _style The color style to set
		virtual void setColorStyle(aColorStyle * _style) override;

		// ################################################################################################################

		// Item adder

		//! @brief Will add a new button to the dialog
		//! Will return the buttons local UID
		//! @param _buttonText The button text
		UID addButton(const QString& _buttonText);

		//! @brief Will add a new text input
		//! Will return the inputs local UID
		//! @param _label The label text
		//! @param _initialText The initial text of the text edit
		//! @param _placeholder The placeholder text
		//! @param _toolTip The tooltip text
		UID addTextInput(const QString& _label, const QString& _initialText, const QString& _placeholder = QString(), const QString& _toolTip = QString());

		//! @brief Will add a new text input as a password input
		//! Will return the inputs local UID
		//! @param _label The label text
		//! @param _initialText The initial text of the text edit
		//! @param _placeholder The placeholder text
		//! @param _toolTip The tooltip text
		UID addPasswordInput(const QString& _label, const QString& _initialText, const QString& _placeholder = QString(), const QString& _toolTip = QString());

		//! @brief Will add a new date input
		//! Will return the inputs local UID
		//! @param _label The label text
		//! @param _initialDate The initial date to set
		UID addDateInput(const QString& _label, const aDate& _initialDate, dateFormat _dateFormat);

		//! @brief Will add a new time input
		//! Will return the inputs local UID
		//! @param _label The label text
		//! @param _initialTime The initial time to set
		UID addTimeInput(const QString& _label, const aTime& _initialTime, timeFormat _timeFormat);

		//! @brief Will add a new checkbox input
		//! Will return the inputs local UID
		//! @param _label The label text
		//! @param _initiallyChecked The initial checked state
		UID addCheckInput(const QString& _label, bool _initiallyChecked);

		//! @brief Will add a new multiline text input
		//! Will return the inputs local UID
		//! @param _label The label text
		//! @param _initialText The initial text of the input
		UID addMultilineTextInput(const QString& _label, const QString& _initialText = QString(), const QString& _placeholderText = QString());

		//! @brief Will add the custom widget
		//! Will return the inputs local UID.
		//! The dialog will take over the widget.
		//! @param _label The label text
		//! @param _widget The widget to add
		UID addCustomInput(const QString& _label, aWidget * _widget);

		// ################################################################################################################

		// Getter

		//! @brief Will return the current text in a text based input
		//! @param _inputUID The input UID
		QString getText(UID _inputUID);

		//! @brief Will return the current text in a text based input
		//! The text will be hashed and the hashed string will be returned
		//! @param _inputUID The input UID
		QString getPassword(UID _inputUID, HashAlgorithm _hashAlgorithm);

		//! @brief Will return the current date
		//! @param _inputUID The input UID
		aDate getDate(UID _inputUID);

		//! @brief Will return the current time
		//! @param _inputUID The input UID
		aTime getTime(UID _inputUID);

		//! @brief Will return the current checked state
		//! @param _inputUID The input UID
		bool getChecked(UID _inputUID);

		//! @brief Will return the input with the specified UID
		//! @param _inputUID The input UID
		aWidget * getInput(UID _inputUID);

		// ################################################################################################################

		// Setter

		//! @brief Will set the current text in a text based input
		//! @param _inputUID The input UID
		void setText(UID _inputUID, const QString& _text);

		//! @brief Will set the current date
		//! @param _inputUID The input UID
		void setDate(UID _inputUID, const aDate& _date);

		//! @brief Will set the current time
		//! @param _inputUID The input UID
		void setTime(UID _inputUID, const aTime& _time);

		//! @brief Will set the current checked state
		//! @param _inputUID The input UID
		void setChecked(UID _inputUID, bool _isChecked);

	signals:
		void buttonClicked(UID _buttonUID);
		void editFinished(UID _editUID);

	protected slots:
		void slotButtonClicked(void);
		void slotInputChanged(void);

	protected:

		std::vector<aPushButtonWidget *>		m_buttons;			//! All buttons that are placed in this dialog
		std::map<UID, std::pair<aLabelWidget *, 
			aWidget *>>							m_inputs;			//! All inputs that are placed in this dialog

		UID										m_currentUid;		//! The current item UID
		int										m_currentInputRow;	//! The current row index

		QVBoxLayout *							m_centralLayout;	//! The central layout of the dialog

		QWidget *								m_inputsLayoutW;	//! The layout widget for the inputs
		QGridLayout *							m_inputsLayout;		//! The inputs layout

		QWidget *								m_buttonLayoutW;	//! The layout widget for the buttons
		QHBoxLayout *							m_buttonLayout;		//! The buttons layout

	private:

		aCustomizableInputDialog(aCustomizableInputDialog&) = delete;
		aCustomizableInputDialog& operator = (aCustomizableInputDialog&) = delete;

	};

}