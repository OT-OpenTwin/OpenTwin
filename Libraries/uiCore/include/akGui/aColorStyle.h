/*
 *	File:		aColorStyle.h
 *	Package:	akGui
 *
 *  Created on: April 09, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// C++ header
#include <vector>				// vector<>

// Qt header
#include <qstring.h>			// QString

// AK header
#include <akCore/globalDataTypes.h>	// Global data types
#include <akCore/akCore.h>	// Global data types
#include <akGui/aColor.h>			// Color

namespace ak {

	//! @brief This class is used for storing aColor information used to paint the UI
	//! This class is an abstract class. It defines the main functions used by the UI_manager and all its childs
	//! This object provides the main foreground aColor and background aColor for the window and the controls
	class UICORE_API_EXPORT aColorStyle {
	public:

		aColorStyle() {}

		//! @brief Will return the name of this styleSheet
		virtual QString getColorStyleName(void) const = 0;

		//! @brief Will return a stylesheet that can be applied
		//! If the aColor areas foreground and background are provided the stylesheet will look something like this:
		//!			color:#00ffd0; background-color:#ffffff;
		//! A prefix can be added to specify the widget
		//! When _prefix = "QLabel{" then the stylehseet will now look like this
		//!			QLabel{color:#00ffd0; background-color:#ffffff;
		//! Now a suffix would make sense.
		//! When _suffix = "}\n" then the stylesheet will now look like this
		//!			QLabel{color:#00ffd0; background-color:#ffffff;}\n
		//!
		//! @param _colorAreas The ares to add to the stylesheet
		//! @param _prefix The prefix to add to the stylesheet
		//! @param _suffix The suffix to add to the stylesheet
		virtual QString toStyleSheet(
			colorAreaFlag				_colorAreas,
			const QString &				_prefix = QString(),
			const QString &				_suffix = QString()
		) = 0;

		//! @brief Returns the currently set window main foreground color
		aColor getWindowMainForegroundColor(void) const { return m_windowMainForecolor; }

		//! @brief Returns the currently set window main background color
		aColor getWindowMainBackgroundColor(void) const { return m_windowMainBackcolor; }

		//! @brief Returns the currently set controls main foreground color
		aColor getControlsMainForegroundColor(void) const { return m_controlsMainForecolor; }

		//! @brief Returns the currently set controls main background color
		aColor getControlsMainBackgroundColor(void) const { return m_controlsMainBackcolor; }

		//! @brief Returns the currently set controls mian error foreground color
		aColor getControlsErrorFrontForegroundColor(void) const { return m_controlsErrorFrontForecolor; }

		//! @brief Returns the currently set controls mian error foreground color
		aColor getControlsErrorBackForegroundColor(void) const { return m_controlsErrorBackForecolor; }

		//! @brief Returns the currently set controls mian error foreground color
		aColor getControlsErrorBackBackgroundColor(void) const { return m_controlsErrorBackBackcolor; }

		//! @brief Returns the currently set controls focus background color
		aColor getControlsFocusedBackgroundColor(void) const { return m_controlsFocusBackColor; }

		//! @brief Returns the currently set controls focus color
		aColor getControlsFocusedForegroundColor(void) const { return m_controlsFocusForeColor; }

		//! @brief Returns the currently set controls pressed color
		aColor getControlsPressedForegroundColor(void) const { return m_controlsPressedForeColor; }

		//! @brief Returns the currently set controls pressed color
		aColor getControlsPressedBackgroundColor(void) const { return m_controlsPressedBackColor; }

		//! @brief Returns the currently set alternate fore color
		aColor getAlternateForegroundColor(void) const { return m_alternateForeColor; }

		//! @brief Returns the currently set alternate back color
		aColor getAlternateBackgroundColor(void) const { return m_alternateBackColor; }

		//! @brief Returns the currently set header foreground color
		aColor getHeaderForegroundColor(void) const { return m_headerForeColor; }

		//! @brief Returns the currently set header background color
		aColor getHeaderBackgroundColor(void) const { return m_headerBackColor; }

		//! @brief Returns the currently set controls border color
		aColor getControlsBorderColor(void) const { return m_controlsBorderColor; }

		//! @brief Returns the currently set view background color
		aColor getViewBackgroundColor(void) const { return m_viewBackgroundColor; }

		//! @brief Reeurns the currently set selected background color
		aColor getSelectedBackgroundColor(void) const { return m_controlsSelectedBackColor; }

		//! @brief Returns the currently set selected foreground color
		aColor getSelectedForegroundColor(void) const { return m_controlsSelectedForeColor; }

		//! @brief Will set the provided directories to the colorStyle
		//! @param _directory The new search directory to add
		void setDirectories(
			const std::vector<QString> &	_directories
		) {
			m_directories = _directories;
		}

		//! @brief Returns the filename for the requested file by searching the set directories
		//! @param _fileName The filename only of the requsted file (subdirectories have to be provided)
		//! @throw ak::Exception if the file does not exist (with exception type = FileNotFound)
		virtual QString getFilePath(
			const QString &			_fileName
		) const;

	protected:

		aColor					m_windowMainForecolor;			//! The currently set window main foreground color
		aColor					m_windowMainBackcolor;			//! The currently set window main background color

		aColor					m_controlsMainForecolor;		//! The currently set controls main foreground color
		aColor					m_controlsMainBackcolor;		//! The currently set controls main background color

		aColor					m_controlsErrorFrontForecolor;	//! The currently set controls error front foreground color
		aColor					m_controlsErrorBackForecolor;	//! The currently set controls error back foreground color
		aColor					m_controlsErrorBackBackcolor;	//! The currently set controls error back background color
		aColor					m_controlsFocusBackColor;		//! The currently set controls focus back color
		aColor					m_controlsFocusForeColor;		//! The currently set controls focus fore color
		aColor					m_controlsPressedBackColor;	//! The currently set controls pressed color
		aColor					m_controlsPressedForeColor;	//! The currently set controls pressed color
		aColor					m_controlsBorderColor;

		aColor					m_viewBackgroundColor;

		aColor					m_alternateBackColor;
		aColor					m_alternateForeColor;

		aColor					m_headerBackColor;
		aColor					m_headerForeColor;
		aColor					m_headerBorderColor;

		aColor					m_controlsSelectedBackColor;
		aColor					m_controlsSelectedForeColor;

		std::vector<QString>	m_directories;					//! Contains all directories to scan for incons
	};
} // namespace ak