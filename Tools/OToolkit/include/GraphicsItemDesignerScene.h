// @otlicense
// File: GraphicsItemDesignerScene.h
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
#include "GraphicsItemDesignerImageExportConfig.h"

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"

// Qt header
#include <QtGui/qimage.h>

class GraphicsItemDesignerView;

class GraphicsItemDesignerScene : public ot::GraphicsScene {
	OT_DECL_NOCOPY(GraphicsItemDesignerScene)
	OT_DECL_NODEFAULT(GraphicsItemDesignerScene)
public:
	enum Mode {
		NoMode,
		PointPickingMode
	};

	GraphicsItemDesignerScene(GraphicsItemDesignerView* _view);
	virtual ~GraphicsItemDesignerScene();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void enablePickingMode(void);
	void disablePickingMode(void);

	void setItemSize(const QSizeF& _size);
	const QSizeF& getItemSize(void) const { return m_itemSize; };

	QImage exportAsImage(const GraphicsItemDesignerImageExportConfig& _exportConfig);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected: Event handler
protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
	virtual void drawBackground(QPainter* _painter, const QRectF& _rect) override;

private:
	void constrainItemToScene(QGraphicsItem* _item);

	GraphicsItemDesignerView* m_view;
	Mode m_mode;
	QSizeF m_itemSize;
};