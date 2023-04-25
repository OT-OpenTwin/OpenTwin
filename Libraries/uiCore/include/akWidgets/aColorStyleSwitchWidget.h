/*
 *	File:		aColorStyleSwitchWidget.h
 *	Package:	akWidgets
 *
 *  Created on: June 30, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

#include <akCore/globalDataTypes.h>
#include <akWidgets/aWidget.h>
#include <akWidgets/aToolButtonWidget.h>

#include <qicon.h>
#include <qstring.h>

class QAction;

namespace ak {

	class UICORE_API_EXPORT aColorStyleSwitchWidget : public aToolButtonWidget {
		Q_OBJECT
	public:
		aColorStyleSwitchWidget(
			const QString &			_brightModeTitle,
			const QString &			_darkModeTitle,
			const QIcon &			_brightModeIcon,
			const QIcon &			_darkModeIcon,
			bool					_isBright = true
		);
		virtual ~aColorStyleSwitchWidget();

		// #######################################################################################################
		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		//! @brief Will set the objects color style
		//! @param _colorStyle The color style to set
		//! @throw ak::Exception if the provided color style is a nullptr or failed to repaint the object
		virtual void setColorStyle(
			aColorStyle *			_colorStyle
		) override;

		// #######################################################################################################

		void setAutoSetColorStyle(bool _enabled) { m_isAutoSetColorStyle = _enabled; }

		bool isAutoSetColorStyle(void) const { return m_isAutoSetColorStyle; }

		void setCurrentIsBright(bool _isBright);

		bool isCurrentBright(void) const { return m_isBright; }

		void setTitles(const QString& _brightModeTitle, const QString& _darkModeTitle);

	signals:
		void changed(void);

	private slots:
		void slotClicked(bool _checked);

	private:

		QAction *	m_action;
		QString		m_brightModeTitle;
		QString		m_darkModeTitle;
		QIcon		m_brightModeIcon;
		QIcon		m_darkModeIcon;
		bool		m_isBright;
		bool		m_isAutoSetColorStyle;

		aColorStyleSwitchWidget() = delete;
		aColorStyleSwitchWidget(aColorStyleSwitchWidget&) = delete;
		aColorStyleSwitchWidget& operator = (aColorStyleSwitchWidget&) = delete;
	};
}