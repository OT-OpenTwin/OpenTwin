/*
 *	File:		aSpecialTabBar.h
 *	Package:	akGui
 *
 *  Created on: April 04, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // C++ header
#include <vector>

// Qt header
#include <qtabbar.h>

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/aObject.h>
#include <akGui/aColor.h>

// Forward declaration
class QWidget;

namespace ak {

	class UICORE_API_EXPORT aSpecialTabBar : public QTabBar, public aObject {
		Q_OBJECT
	public:
		aSpecialTabBar();
		virtual ~aSpecialTabBar();

		virtual void paintEvent(QPaintEvent * _event) override;

		void clearColors(bool _repaint = true);

		void clearColor(int _index, bool _repaint = true);

		void addColor(int _index, const aColor & _color, bool _repaint = true);

		void setRepaintBlocked(bool _blocked = true) { m_repaintIsBlocked = _blocked; }

	private:

		std::map<int, aColor>		m_colors;

		bool						m_repaintIsBlocked;

		aSpecialTabBar(const aSpecialTabBar&) = delete;
		aSpecialTabBar & operator = (const aSpecialTabBar&) = delete;

	};
} // namespace ak