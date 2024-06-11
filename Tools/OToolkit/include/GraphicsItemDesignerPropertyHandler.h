//! @file GraphicsItemDesignerPropertyHandler.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyInputInt.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/PropertyInputInt.h"
#include "OTWidgets/PropertyInputBool.h"
#include "OTWidgets/PropertyInputColor.h"
#include "OTWidgets/PropertyInputDouble.h"
#include "OTWidgets/PropertyInputString.h"
#include "OTWidgets/PropertyInputPainter2D.h"
#include "OTWidgets/PropertyInputStringList.h"

// Qt header
#include <QtCore/qobject.h>

class QTreeWidgetItem;
class GraphicsItemDesignerNavigation;

//! \class GraphicsItemDesignerPropertyHandler
//! \brief The GraphicsItemDesignerPropertyHandler is used to receive property grid notifications and requests.
class GraphicsItemDesignerPropertyHandler : public QObject {
	Q_OBJECT
	OT_DECL_NOCOPY(GraphicsItemDesignerPropertyHandler)
public:
	GraphicsItemDesignerPropertyHandler();
	virtual ~GraphicsItemDesignerPropertyHandler();

	void setNavigation(GraphicsItemDesignerNavigation* _navigation) { m_navigation = _navigation; };
	GraphicsItemDesignerNavigation* getNavigation(void) const { return m_navigation; };

	void setNavigationItem(QTreeWidgetItem* _item) { m_navigationItem = _item; };
	QTreeWidgetItem* getNavigationItem(void) const { return m_navigationItem; };

	void setPropertyGrid(ot::PropertyGrid* _grid);
	void unsetPropertyGrid(void);
	ot::PropertyGrid* getPropertyGrid(void) const { return m_propertyGrid; };

	virtual void itemAboutToBeDestroyed(void) {};

protected:
	//! \brief Is called when the property grid needs to be filled.
	virtual void fillPropertyGrid(void) = 0;


	virtual void propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) = 0;
	virtual void propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) = 0;

private Q_SLOTS:
	void slotPropertyChanged(const std::string& _group, const std::string& _item);
	void slotPropertyChanged(const std::list<std::string>& _groupPath, const std::string& _item);
	void slotPropertyDeleteRequested(const std::string& _group, const std::string& _item);
	void slotPropertyDeleteRequested(const std::list<std::string>& _groupPath, const std::string& _item);

private:
	QTreeWidgetItem* m_navigationItem;
	ot::PropertyGrid* m_propertyGrid;
	GraphicsItemDesignerNavigation* m_navigation;
};