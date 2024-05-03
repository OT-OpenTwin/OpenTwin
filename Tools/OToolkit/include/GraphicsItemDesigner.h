//! @file GraphicsItemDesigner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OTCore/OTClassHelper.h"
#include "OToolkitAPI/Tool.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot { class PropertyGrid; };
namespace ot { class GraphicsView; };

class GraphicsItemDesigner : public QObject, public otoolkit::Tool {
public:
	GraphicsItemDesigner();
	virtual ~GraphicsItemDesigner() {};

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override { return QString("Graphics Item Designer"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	ot::GraphicsView* m_view;
	ot::PropertyGrid* m_props;
};
