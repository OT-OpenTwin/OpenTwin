//! @file GraphicsItemDesignerNavigationRoot.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerPropertyHandler.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

class GraphicsItemDesignerView;

class GraphicsItemDesignerNavigationRoot : public QTreeWidgetItem, public GraphicsItemDesignerPropertyHandler {
public:
	enum ExportConfigFlag {
		NoFlags = 0x00,
		AutoAlign = 0x01,
		MoveableItem = 0x02
	 };
	typedef ot::Flags<ExportConfigFlag> ExportConfigFlags;

	GraphicsItemDesignerNavigationRoot();
	virtual ~GraphicsItemDesignerNavigationRoot() {};

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setDesignerView(GraphicsItemDesignerView* _view) { m_view = _view; };

	void setExportConfigFlag(ExportConfigFlag _flag, bool _active = true) { m_exportConfigFlags.setFlag(_flag, _active); };
	void setExportConfigFlags(const ExportConfigFlags& _flags) { m_exportConfigFlags = _flags; };
	const ExportConfigFlags& getExportConfigFlags(void) const { return m_exportConfigFlags; };

protected:
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;
	virtual void propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;

private:
	GraphicsItemDesignerView* m_view;
	ExportConfigFlags m_exportConfigFlags;
};

OT_ADD_FLAG_FUNCTIONS(GraphicsItemDesignerNavigationRoot::ExportConfigFlag)