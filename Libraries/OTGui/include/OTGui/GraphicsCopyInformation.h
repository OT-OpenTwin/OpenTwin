//! @file GraphicsCopyInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/CopyInformation.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT GraphicsCopyInformation : public CopyInformation {
	public:
		struct ItemInformation {
			UID uid;
			Point2DD pos;
		};

		GraphicsCopyInformation() = default;
		GraphicsCopyInformation(const GraphicsCopyInformation&) = default;
		GraphicsCopyInformation(GraphicsCopyInformation&&) = default;
		virtual ~GraphicsCopyInformation() = default;

		GraphicsCopyInformation& operator = (const GraphicsCopyInformation&) = default;
		GraphicsCopyInformation& operator = (GraphicsCopyInformation&&) = default;

		static std::string getGraphicsCopyInformationType(void) { return "GraphicsCopy"; };
		virtual std::string getCopyType(void) const override { return GraphicsCopyInformation::getGraphicsCopyInformationType(); };

		virtual void addToJsonObject(ot::JsonValue & _object, ot::JsonAllocator & _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setViewName(const std::string& _name) { m_viewName = _name; };
		const std::string& getViewName(void) const { return m_viewName; };

		void setViewOwner(const BasicServiceInformation& _owner) { m_viewOwner = _owner; };
		const BasicServiceInformation& getViewOwner(void) const { return m_viewOwner; };

		void addItemInformation(UID _uid, const Point2DD& _pos);
		void addItemInformation(const ItemInformation& _info);
		void setItemInformation(const std::list<ItemInformation>& _info) { m_items = _info; };
		const std::list<ItemInformation>& getItemInformation(void) const { return m_items; };

		//! @brief Move all items to this point by keeping the relative item positions.
		void moveItemsToPoint(const Point2DD& _pos);

		//! @brief Move all items by the specified distance.
		void moveItemsBy(const Point2DD& _dist);

		bool isValid(void) const;

	private:
		std::string m_viewName;
		BasicServiceInformation m_viewOwner;
		std::list<ItemInformation> m_items;
	};

}