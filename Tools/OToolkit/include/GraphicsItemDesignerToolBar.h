//! @file GraphicsItemDesignerToolBar.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesigner.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <list>

class QGridLayout;

class GraphicsItemDesignerToolBar : public QObject, public ot::QWidgetInterface {
	Q_OBJECT
public:
	GraphicsItemDesignerToolBar(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerToolBar();

	virtual QWidget* getQWidget(void) override { return m_widget; };

	virtual bool eventFilter(QObject* _obj, QEvent* _event) override;

Q_SIGNALS:
	void modeRequested(GraphicsItemDesigner::DesignerMode _mode);

	// ###########################################################################################################################################################################################################################################################################################################################

private Q_SLOTS:
	void slotLine(void);
	void slotSquare(void);
	void slotRect(void);
	void slotTriangle(void);
	void slotPolygon(void);
	void slotShape(void);

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	void rearrangeWidgets(void);

	QWidget* m_widget;
	QGridLayout* m_optionsLayout;
	GraphicsItemDesigner* m_designer;

	std::list<QWidget* > m_options;
};