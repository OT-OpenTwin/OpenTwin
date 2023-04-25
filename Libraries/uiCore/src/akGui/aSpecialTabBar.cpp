/*
 *	File:		aSpecialTabBar.cpp
 *	Package:	akGui
 *
 *  Created on: April 04, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akGui/aSpecialTabBar.h>

#include <qstylepainter.h>
#include <QStyleOptionTab>

ak::aSpecialTabBar::aSpecialTabBar() : aObject{ otSpecialTabBar }, m_repaintIsBlocked{ false } {}

ak::aSpecialTabBar::~aSpecialTabBar() { A_OBJECT_DESTROYING }

void ak::aSpecialTabBar::paintEvent(QPaintEvent * _event) {

	QStylePainter painter(this);
	QStyleOptionTab opt;

	for (int i = 0; i < count(); i++)
	{
		initStyleOption(&opt, i);
		auto itm = m_colors.find(i);
		if (itm != m_colors.end()) {
			opt.palette.setColor(QPalette::Button, itm->second.toQColor());
		}
		painter.drawControl(QStyle::CE_TabBarTabShape, opt);
		painter.drawControl(QStyle::CE_TabBarTabLabel, opt);
	}

}

void ak::aSpecialTabBar::clearColors(bool _repaint) {
	m_colors.clear();
	if (_repaint && !m_repaintIsBlocked) { repaint(); }
}

void ak::aSpecialTabBar::clearColor(int _index, bool _repaint) {
	m_colors.erase(_index);
	if (_repaint && !m_repaintIsBlocked) { repaint(); }
}

void ak::aSpecialTabBar::addColor(int _index, const aColor & _color, bool _repaint) {
	m_colors.insert_or_assign(_index, _color);
	if (_repaint && !m_repaintIsBlocked) { repaint(); }
}