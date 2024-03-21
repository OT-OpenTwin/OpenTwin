/*
 *	File:		aPromptDialog.h
 *	Package:	akDialogs
 *
 *  Created on: October 07, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>

#include <akGui/aDialog.h>

// Qt header
#include <qobject.h>				// QObject
#include <qstring.h>
#include <qicon.h>

// Forward declaration
class QWidget;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QResizeEvent;

namespace ak {

	class aLabelWidget;
	class aPushButtonWidget;
	class aAbstractDictionary;

	class UICORE_API_EXPORT aPromptDialog : public aDialog {
		Q_OBJECT
	public:
		enum promtDialogControl {
			Ok,
			Yes,
			No,
			Cancel,
			Ignore,
			Retry
		};

		aPromptDialog(
			const QString &			_message,
			const QString &			_title,
			promptType				_type,
			QWidget *				_parent = nullptr
		);

		aPromptDialog(
			const QString &			_message,
			const QString &			_title,
			promptType				_type,
			const QIcon &			_icon,
			QWidget *				_parent = nullptr
		);

		virtual ~aPromptDialog();

		//! @brief Will translate the controls with the specified dictionary
		//! The following translations must be present:
		//!		-  PromptText_Ok
		//!		-  PromptText_Yes
		//!		-  PromptText_No
		//!		-  PromptText_Cancel
		//!		-  PromptText_Ignore
		//!		-  PromptText_Retry
		//! @param _dictionary The dictionary to use to translate the titles
		void translateControls(aAbstractDictionary * _dictionary);

		//! @brief Will return the key for the specified promt dialog control
		static QString translationKey(promtDialogControl _control);

	protected:
		virtual void resizeEvent(QResizeEvent * _event) override;

	private slots:
		void slotOk(void);
		void slotCancel(void);
		void slotYes(void);
		void slotNo(void);
		void slotRetry(void);
		void slotIgnore(void);

	private:

		void setupDialog(
			const QString &			_message,
			const QString &			_title,
			promptType				_type,
			const QIcon &			_icon
		);

		QIcon				m_currentIcon;
		promptType			m_type;
		QVBoxLayout *		m_layout;

		QHBoxLayout *		m_buttonsLayout;
		QWidget *			m_buttonsWidget;

		QHBoxLayout *		m_infoLayout;
		QWidget *			m_infoWidget;

		aLabelWidget *		m_iconLabel;
		aLabelWidget *		m_label;
		aPushButtonWidget *	m_button1;
		aPushButtonWidget *	m_button2;
		aPushButtonWidget *	m_button3;

	};
} // namespace ak