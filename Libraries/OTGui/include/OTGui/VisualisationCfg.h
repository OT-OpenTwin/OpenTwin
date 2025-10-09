#pragma once
#include "OTCore/Serializable.h"
#include "OTCore/CoreTypes.h"
namespace ot
{
	class __declspec(dllexport) VisualisationCfg : public ot::Serializable
	{
	public:
		VisualisationCfg(const std::string& _visualisationType, const bool _setAsActiveView, const ot::UIDList& _visualisingEntities, const bool _overrideViewerContent, const bool _supressViewHandling)
			: m_visualisationType(_visualisationType), m_setAsActiveView(_setAsActiveView), m_visualisingEntities(_visualisingEntities), m_overrideViewerContent(_overrideViewerContent), m_supressViewHandling(_supressViewHandling)
		{
		}

		VisualisationCfg() = default;
		~VisualisationCfg() = default;
		VisualisationCfg(const VisualisationCfg& _other) = default;
		VisualisationCfg(VisualisationCfg&& _other) = default;
		VisualisationCfg& operator=(const VisualisationCfg& _other) = default;
		VisualisationCfg& operator=(VisualisationCfg&& _other) = default;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setAsActiveView(bool _setAsActiveView) { m_setAsActiveView = _setAsActiveView; }
		void setVisualisingEntities(const ot::UIDList& _visualisingEntities) { m_visualisingEntities = _visualisingEntities; }
		void addVisualisingEntity(ot::UID _visualisingEntity) { m_visualisingEntities.push_back(_visualisingEntity); }
		void setOverrideViewerContent(bool _overrideViewerContent) { m_overrideViewerContent = _overrideViewerContent; }
		void setSupressViewHandling(bool _supressViewHandling) { m_supressViewHandling = _supressViewHandling; }
		void setVisualisationType(const std::string _visualisationType) { m_visualisationType = _visualisationType; }

		bool getSetAsActiveView() const { return m_setAsActiveView; }
		bool getOverrideViewerContent() const { return m_overrideViewerContent; }
		bool getSupressViewHandling() const { return m_supressViewHandling; }
		const ot::UIDList& getVisualisingEntities() const { return m_visualisingEntities; }
		const std::string getVisualisationType() const { return m_visualisationType; }


	private:
		bool m_setAsActiveView = true;
		ot::UIDList m_visualisingEntities;
		bool m_overrideViewerContent = true;
		bool m_supressViewHandling = false;
		std::string m_visualisationType = "";
	};
}

