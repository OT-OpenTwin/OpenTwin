//! @file GraphicsItemDesignerPropertyHandler.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyGrid.h"

// Qt header
#include <QtCore/qobject.h>

class GraphicsItemDesignerPropertyHandler : public QObject {
	Q_OBJECT
public:
	GraphicsItemDesignerPropertyHandler();
	virtual ~GraphicsItemDesignerPropertyHandler() {};

	void setPropertyGrid(ot::PropertyGrid* _grid);
	void unsetPropertyGrid(void);
	ot::PropertyGrid* getPropertyGrid(void) const { return m_propertyGrid; };

protected:
	virtual void fillPropertyGrid(void) = 0;
	virtual void propertyChanged(const std::string& _group, const std::string& _item) = 0;
	virtual void propertyDeleteRequested(const std::string& _group, const std::string& _item) = 0;

private Q_SLOTS:
	void slotPropertyChanged(const std::string& _group, const std::string& _item);
	void slotPropertyDeleteRequested(const std::string& _group, const std::string& _item);

private:
	ot::PropertyGrid* m_propertyGrid;
};