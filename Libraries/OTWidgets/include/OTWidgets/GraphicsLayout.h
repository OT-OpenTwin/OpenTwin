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

#define OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem "OT_GraphicsVBoxLayoutItem"
#define OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem "OT_GraphicsHBoxLayoutItem"
#define OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem "OT_GraphicsGridLayoutItem"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsBoxLayout : public QGraphicsLinearLayout, public ot::GraphicsItem {
	public:
		GraphicsBoxLayout(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsBoxLayout() {};

	private:
		GraphicsBoxLayout() = delete;
		GraphicsBoxLayout(const GraphicsBoxLayout&) = delete;
		GraphicsBoxLayout& operator = (const GraphicsBoxLayout&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsVBoxLayout : public GraphicsBoxLayout {
	public:
		GraphicsVBoxLayout(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsVBoxLayout() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem); };

		virtual void setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:

		GraphicsVBoxLayout(const GraphicsVBoxLayout&) = delete;
		GraphicsVBoxLayout& operator = (const GraphicsVBoxLayout&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsHBoxLayout : public GraphicsBoxLayout {
	public:
		GraphicsHBoxLayout(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsHBoxLayout() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem); };

		virtual void setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		GraphicsHBoxLayout(const GraphicsHBoxLayout&) = delete;
		GraphicsHBoxLayout& operator = (const GraphicsHBoxLayout&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsGridLayout : public QGraphicsGridLayout, public ot::GraphicsItem {
	public:
		GraphicsGridLayout(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsGridLayout() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem); };

		virtual void setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		GraphicsGridLayout(const GraphicsGridLayout&) = delete;
		GraphicsGridLayout& operator = (const GraphicsGridLayout&) = delete;
	};

}