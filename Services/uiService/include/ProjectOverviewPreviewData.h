// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ProjectInformation.h"
#include "OTWidgets/WidgetTypes.h"

// std header
#include <vector>

namespace ot {

	class ProjectOverviewPreviewData : public ProjectInformation {
		OT_DECL_DEFCOPY(ProjectOverviewPreviewData)
		OT_DECL_DEFMOVE(ProjectOverviewPreviewData)
	public:
		ProjectOverviewPreviewData();
		ProjectOverviewPreviewData(const ProjectInformation& _basicInfo);
		virtual ~ProjectOverviewPreviewData() = default;
		
		void setImageData(const std::vector<char>& _data, ImageFileFormat _format) { m_imageData = _data; m_imageFormat = _format; };
		void setImageData(std::vector<char>&& _data, ImageFileFormat _format) { m_imageData = std::move(_data); m_imageFormat = _format; };
		const std::vector<char>& getImageData() const { return m_imageData; };
		ImageFileFormat getImageFormat() const { return m_imageFormat; };

		void setDescription(const std::string& _description) { m_description = _description; };
		const std::string& getDescription() const { return m_description; };

	private:
		std::vector<char> m_imageData;
		ImageFileFormat m_imageFormat;
		std::string m_description;
	};

}