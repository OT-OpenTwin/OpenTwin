//! @file GraphicsLayout.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtWidgets/qgraphicslinearlayout.h>
#include <QtWidgets/qgraphicsgridlayout.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem "OT_GILayV"
#define OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem "OT_GILayH"
#define OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem "OT_GILayG"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsBoxLayoutItem : public QGraphicsLinearLayout, public ot::GraphicsItem {
	public:
		GraphicsBoxLayoutItem(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsBoxLayoutItem() {};

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		GraphicsBoxLayoutItem() = delete;
		GraphicsBoxLayoutItem(const GraphicsBoxLayoutItem&) = delete;
		GraphicsBoxLayoutItem& operator = (const GraphicsBoxLayoutItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsVBoxLayoutItem : public GraphicsBoxLayoutItem {
	public:
		GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsVBoxLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		QSizeF m_size;

		GraphicsVBoxLayoutItem(const GraphicsVBoxLayoutItem&) = delete;
		GraphicsVBoxLayoutItem& operator = (const GraphicsVBoxLayoutItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsHBoxLayoutItem : public GraphicsBoxLayoutItem {
	public:
		GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsHBoxLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		QSizeF m_size;

		GraphicsHBoxLayoutItem(const GraphicsHBoxLayoutItem&) = delete;
		GraphicsHBoxLayoutItem& operator = (const GraphicsHBoxLayoutItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsGridLayoutItem : public QGraphicsGridLayout, public ot::GraphicsItem {
	public:
		GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsGridLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		QSizeF m_size;

		GraphicsGridLayoutItem(const GraphicsGridLayoutItem&) = delete;
		GraphicsGridLayoutItem& operator = (const GraphicsGridLayoutItem&) = delete;
	};

}