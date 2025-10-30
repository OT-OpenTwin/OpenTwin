// @otlicense
// File: GraphicsItemDesignerToolBar.h
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

#pragma once

// OToolkit header
#include "GraphicsItemDesignerDrawHandler.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtWidgets/qtoolbar.h>

// std header
#include <list>

class QGridLayout;
class GraphicsItemDesigner;

class GraphicsItemDesignerToolBar : public QToolBar {
	Q_OBJECT
public:
	GraphicsItemDesignerToolBar(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerToolBar();

Q_SIGNALS:
	void modeRequested(GraphicsItemDesignerDrawHandler::DrawMode _mode);
	void clearRequested(void);
	void importRequested(void);
	void exportRequested(void);
	void itemUpdateRequested(void);
	void exportAsImageRequested(void);
	void makeTransparentRequested(void);
	void duplicateRequested(void);
	void previewRequested(void);

	// ###########################################################################################################################################################################################################################################################################################################################

private Q_SLOTS:
	void slotImport(void);
	void slotExport(void);
	void slotExportAsImage(void);
	void slotGeneratePreview(void);
	void slotItemUpdateRequested(void);

	void slotLine(void);
	void slotSquare(void);
	void slotRect(void);
	void slotCircle(void);
	void slotEllipse(void);
	void slotArc(void);
	void slotTriangle(void);
	void slotPolygon(void);
	void slotShape(void);
	void slotText(void);

	void slotMakeTransparent(void);
	void slotDuplicate(void);
	void slotClear(void);

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	GraphicsItemDesigner* m_designer;
};