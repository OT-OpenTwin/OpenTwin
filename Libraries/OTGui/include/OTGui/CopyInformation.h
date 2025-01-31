//! @file CopyInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/WidgetViewBase.h"
#include "OTGui/CopyEntityInformation.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT CopyInformation : public Serializable {
	public:
		static std::string getCopyVersionJsonKey(void);

		CopyInformation();
		CopyInformation(const ConstJsonObject& _jsonObject);
		CopyInformation(const CopyInformation&) = default;
		CopyInformation(CopyInformation&&) = default;
		~CopyInformation() = default;

		CopyInformation& operator = (const CopyInformation&) = default;
		CopyInformation& operator = (CopyInformation&&) = default;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		int getCopyVersion(void) const { return 1; };

		void setOriginProjectName(const std::string& _projectName) { m_originProjectName = _projectName; };
		const std::string& getOriginProjectName(void) const { return m_originProjectName; };

		void setOriginViewInfo(const WidgetViewBase& _info) { m_originViewInfo = _info; };
		const WidgetViewBase& getOriginViewInfo(void) const { return m_originViewInfo; };

		void setDestinationViewInfo(const WidgetViewBase& _info) { m_destinationViewInfo = _info; };
		const WidgetViewBase& getDestinationViewInfo(void) const { return m_destinationViewInfo; };

		void setDestinationScenePos(const Point2DD& _pos) { m_destinationScenePos = _pos; m_destinationScenePosSet = true; };
		const Point2DD& getDestinationScenePos(void) const { return m_destinationScenePos; };
		bool getDestinationScenePosSet(void) const { return m_destinationScenePosSet; };

		void addEntity(UID _uid, const std::string& _name, const std::string& _rawData = std::string());
		void addEntity(const CopyEntityInformation& _entityInfo) { m_entities.push_back(_entityInfo); };
		void setEntities(const std::list<CopyEntityInformation>& _entities) { m_entities = _entities; };
		std::list<CopyEntityInformation>& getEntities(void) { return m_entities; };
		const std::list<CopyEntityInformation>& getEntities(void) const { return m_entities; };
		void clearEntities(void) { m_entities.clear(); };

	private:
		std::string m_originProjectName;
		
		WidgetViewBase m_originViewInfo;
		WidgetViewBase m_destinationViewInfo;
		
		Point2DD m_destinationScenePos;
		bool m_destinationScenePosSet;

		std::list<CopyEntityInformation> m_entities;
	};

}