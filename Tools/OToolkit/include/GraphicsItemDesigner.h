//! @file GraphicsItemDesigner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerDrawHandler.h"
#include "GraphicsItemDesignerExportConfig.h"
#include "GraphicsItemDesignerImageExportConfig.h"

// ToolkitAPI header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;
class GraphicsItemDesignerView;
class GraphicsItemDesignerToolBar;
class GraphicsItemDesignerPreview;
class GraphicsItemDesignerNavigation;
class GraphicsItemDesignerDrawHandler;
namespace ot { class PropertyGrid; };
namespace ot { class GraphicsItemCfg; };

class GraphicsItemDesigner : public QObject, public otoolkit::Tool {
	Q_OBJECT
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

	virtual void restoreToolSettings(QSettings& _settings) override;

	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	GraphicsItemDesignerView* getView(void) const { return m_view; };
	ot::PropertyGrid* getPropertyGrid(void) const { return m_props; };
	GraphicsItemDesignerToolBar* getToolBar(void) const { return m_toolBar; };
	GraphicsItemDesignerNavigation* getNavigation(void) const { return m_navigation; };

	void setExportConfig(const GraphicsItemDesignerExportConfig& _config) { m_exportConfig = _config; };
	const GraphicsItemDesignerExportConfig& getExportConfig(void) const { return m_exportConfig; };

	void setImageExportConfig(const GraphicsItemDesignerImageExportConfig& _config) { m_imageExportConfig = _config; };
	const GraphicsItemDesignerImageExportConfig& getImageExportConfig(void) const { return m_imageExportConfig; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotDrawRequested(GraphicsItemDesignerDrawHandler::DrawMode _mode);
	
	void slotImportRequested(void);
	void slotExportRequested(void);
	void slotExportAsImageRequested(void);
	void slotUpdateConfigRequested(void);
	void slotDrawFinished(void);
	void slotDrawCancelled(void);
	void slotMakeTransparentRequested(void);
	void slotDuplicateRequested(void);
	void slotClearRequested(void);
	void slotGeneratePreview(void);
	void slotDeleteItemsRequested(const ot::UIDList& _items, const ot::UIDList& _connections);

private:
	void createItemFromConfig(const ot::GraphicsItemCfg* _config, bool _keepName);

	GraphicsItemDesignerExportConfig m_exportConfig;
	GraphicsItemDesignerImageExportConfig m_imageExportConfig;

	GraphicsItemDesignerView* m_view;
	ot::PropertyGrid* m_props;
	GraphicsItemDesignerToolBar* m_toolBar;
	GraphicsItemDesignerNavigation* m_navigation;
	GraphicsItemDesignerDrawHandler* m_drawHandler;
	GraphicsItemDesignerPreview* m_preview;
};

