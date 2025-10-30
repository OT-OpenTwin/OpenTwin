// @otlicense
// File: GraphicsItemDesignerToolBar.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// ToolkitAPI header
#include "GraphicsItemDesignerToolBar.h"

// OpenTwin header
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qaction.h>

GraphicsItemDesignerToolBar::GraphicsItemDesignerToolBar(GraphicsItemDesigner* _designer)
	: m_designer(_designer)
{
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Import.png"));
		btn->setText("Import (Ctrl + O)");
		btn->setShortcut(QKeySequence("Ctrl+O"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotImport);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Export.png"));
		btn->setText("Export (Ctrl + S)");
		btn->setShortcut(QKeySequence("Ctrl+S"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotExport);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/ExportAsImage.png"));
		btn->setText("Export As Image (Ctrl + Alt + S)");
		btn->setShortcut(QKeySequence("Ctrl+Alt+S"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotExportAsImage);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/UpdateItem.png"));
		btn->setText("Update Item");
		btn->setShortcutVisibleInContextMenu(true);
		btn->setToolTip("Will import the selected config and export it to the same file. May be used to avoid missing members in the config.");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotItemUpdateRequested);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/GeneratePreview.png"));
		btn->setText("Generate Item preview (Ctrl + Shift + B)");
		btn->setShortcut(QKeySequence("Ctrl+Shift+B"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotGeneratePreview);
		this->addAction(btn);
	}

	this->addSeparator();

	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Line.png"));
		btn->setText("Line (Ctrl + 1)");
		btn->setShortcut(QKeySequence("Ctrl+1"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotLine);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Square.png"));
		btn->setText("Square (Ctrl + 2)");
		btn->setShortcut(QKeySequence("Ctrl+2"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotSquare);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Rect.png"));
		btn->setText("Rectangle (Ctrl + 3)");
		btn->setShortcut(QKeySequence("Ctrl+3"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotRect);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Circle.png"));
		btn->setText("Circle (Ctrl + 4)");
		btn->setShortcut(QKeySequence("Ctrl+4"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotCircle);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Ellipse.png"));
		btn->setText("Ellipse (Ctrl + 5)");
		btn->setShortcut(QKeySequence("Ctrl+5"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotEllipse);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Arc.png"));
		btn->setText("Arc (Ctrl + 6)");
		btn->setShortcut(QKeySequence("Ctrl+6"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotArc);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Triangle.png"));
		btn->setText("Triangle (Ctrl + 7)");
		btn->setShortcut(QKeySequence("Ctrl+7"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotTriangle);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Polygon.png"));
		btn->setText("Polygon (Ctrl + 8)");
		btn->setShortcut(QKeySequence("Ctrl+8"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotPolygon);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Shape.png"));
		btn->setText("Shape (Ctrl + 9)");
		btn->setShortcut(QKeySequence("Ctrl+9"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotShape);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Text.png"));
		btn->setText("Text (Ctrl + 0)");
		btn->setShortcut(QKeySequence("Ctrl+0"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotText);
		this->addAction(btn);
	}

	this->addSeparator();

	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/MakeTransparent.png"));
		btn->setText("Make Transparent");
		btn->setShortcut(QKeySequence("Ctrl+T"));
		btn->setShortcutVisibleInContextMenu(true);
		btn->setToolTip("Will make the currently selected items transparent (Ctrl+T)");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotMakeTransparent);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Duplicate.png"));
		btn->setText("Duplicate");
		btn->setShortcut(QKeySequence("Ctrl+D"));
		btn->setShortcutVisibleInContextMenu(true);
		btn->setToolTip("Will duplicated the currently selected items (Ctrl+D)");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotDuplicate);
		this->addAction(btn);
	}

	this->addSeparator();

	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Clear.png"));
		btn->setText("Clear (Ctrl + Del)");
		btn->setShortcut(QKeySequence("Ctrl+Del"));
		btn->setShortcutVisibleInContextMenu(true);
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotClear);
		this->addAction(btn);
	}
}

GraphicsItemDesignerToolBar::~GraphicsItemDesignerToolBar() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void GraphicsItemDesignerToolBar::slotImport(void) {
	Q_EMIT importRequested();
}

void GraphicsItemDesignerToolBar::slotExport(void) {
	Q_EMIT exportRequested();
}

void GraphicsItemDesignerToolBar::slotExportAsImage(void) {
	Q_EMIT exportAsImageRequested();
}

void GraphicsItemDesignerToolBar::slotGeneratePreview(void) {
	Q_EMIT previewRequested();
}

void GraphicsItemDesignerToolBar::slotItemUpdateRequested(void) {
	Q_EMIT itemUpdateRequested();
}

void GraphicsItemDesignerToolBar::slotLine(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Line);
}

void GraphicsItemDesignerToolBar::slotSquare(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Square);
}

void GraphicsItemDesignerToolBar::slotRect(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Rect);
}

void GraphicsItemDesignerToolBar::slotCircle(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Circle);
}

void GraphicsItemDesignerToolBar::slotEllipse(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Ellipse);
}

void GraphicsItemDesignerToolBar::slotArc(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Arc);
}

void GraphicsItemDesignerToolBar::slotTriangle(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Triangle);
}

void GraphicsItemDesignerToolBar::slotPolygon(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Polygon);
}

void GraphicsItemDesignerToolBar::slotShape(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Shape);
}

void GraphicsItemDesignerToolBar::slotText(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Text);
}

void GraphicsItemDesignerToolBar::slotMakeTransparent(void) {
	Q_EMIT makeTransparentRequested();
}

void GraphicsItemDesignerToolBar::slotDuplicate(void) {
	Q_EMIT duplicateRequested();
}

void GraphicsItemDesignerToolBar::slotClear(void) {
	Q_EMIT clearRequested();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper
