//! @file GraphicsItemDesigner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerDrawHandler.h"

// ToolkitAPI header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;
class GraphicsItemDesignerView;
class GraphicsItemDesignerToolBar;
class GraphicsItemDesignerNavigation;
class GraphicsItemDesignerDrawHandler;
namespace ot { class PropertyGrid; };

class GraphicsItemDesigner : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	enum ExportConfigFlag {
		NoFlags      = 0x00,
		AutoAlign    = 0x01,
		MoveableItem = 0x02,
		ItemGridSnap = 0x04
	};
	typedef ot::Flags<ExportConfigFlag> ExportConfigFlags;

	GraphicsItemDesigner();
	virtual ~GraphicsItemDesigner() {};

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override { return QString("Graphics Item Designer"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setExportConfigFlag(ExportConfigFlag _flag, bool _active = true) { m_exportConfigFlags.setFlag(_flag, _active); };
	void setExportConfigFlags(const ExportConfigFlags& _flags) { m_exportConfigFlags = _flags; };
	const ExportConfigFlags& getExportConfigFlags(void) const { return m_exportConfigFlags; };

	GraphicsItemDesignerView* getView(void) const { return m_view; };
	ot::PropertyGrid* getPropertyGrid(void) const { return m_props; };
	GraphicsItemDesignerToolBar* getToolBar(void) const { return m_toolBar; };
	GraphicsItemDesignerNavigation* getNavigation(void) const { return m_navigation; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotDrawRequested(GraphicsItemDesignerDrawHandler::DrawMode _mode);
	void slotClearRequested(void);
	void slotExportRequested(void);
	void slotExportAsImageRequested(void);
	void slotDrawFinished(void);
	void slotDrawCancelled(void);
	void slotMakeTransparentRequested(void);

private:
	QString m_lastExportFile;
	QString m_lastExportImageFile;

	GraphicsItemDesignerView* m_view;
	ot::PropertyGrid* m_props;
	GraphicsItemDesignerToolBar* m_toolBar;
	GraphicsItemDesignerNavigation* m_navigation;
	GraphicsItemDesignerDrawHandler* m_drawHandler;
	ExportConfigFlags m_exportConfigFlags;
};

OT_ADD_FLAG_FUNCTIONS(GraphicsItemDesigner::ExportConfigFlag)