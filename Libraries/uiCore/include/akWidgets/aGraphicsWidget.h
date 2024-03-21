/*
 *	File:		aGraphicsWidget.h
 *	Package:	akWidgets
 *
 *  Created on: August 10, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// AK header
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

// Qt header
#include <qgraphicsview.h>

class QResizeEvent;

namespace ak {
	class UICORE_API_EXPORT aGraphicsWidget : public QGraphicsView, aWidget {
		Q_OBJECT
	public:
		aGraphicsWidget();
		virtual ~aGraphicsWidget();

		virtual QWidget * widget(void) override;

		virtual void resizeEvent(QResizeEvent * _event) override;

		void setHeightForWidthActive(
			bool				_active
		);

	private:
		bool				m_heightForWidth;
	};
}