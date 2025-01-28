//! @file CopyInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CopyEntityInformation.h"
#include "OTCore/BasicServiceInformation.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_CORE_API_EXPORT CopyInformation : public Serializable {
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

		void setProjectName(const std::string& _projectName) { m_projectName = _projectName; };
		const std::string& getProjectName(void) const { return m_projectName; };

		void setViewName(const std::string& _name) { m_viewName = _name; };
		const std::string& getViewName(void) const { return m_viewName; };

		void setViewOwner(const BasicServiceInformation& _owner) { m_serviceInfo = _owner; };
		const BasicServiceInformation& getViewOwner(void) const { return m_serviceInfo; };

		void addEntity(UID _uid, const std::string& _name, const std::string& _rawData = std::string());
		void addEntity(const CopyEntityInformation& _entityInfo) { m_entities.push_back(_entityInfo); };
		void setEntities(const std::list<CopyEntityInformation>& _entities) { m_entities = _entities; };
		std::list<CopyEntityInformation>& getEntities(void) { return m_entities; };
		const std::list<CopyEntityInformation>& getEntities(void) const { return m_entities; };

		void setScenePos(const Point2DD& _pos) { m_scenePos = _pos; m_scenePosSet = true; };
		const Point2DD& getScenePos(void) const { return m_scenePos; };
		bool getScenePosSet(void) const { return m_scenePosSet; };

		virtual bool isValid(void) const;

	private:
		std::string m_projectName;

		std::string m_viewName;
		BasicServiceInformation m_serviceInfo;

		Point2DD m_scenePos;
		bool m_scenePosSet;

		std::list<CopyEntityInformation> m_entities;
	};

}