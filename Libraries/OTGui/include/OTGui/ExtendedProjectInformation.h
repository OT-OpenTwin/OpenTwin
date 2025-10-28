// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ProjectInformation.h"
#include "OTGui/GuiTypes.h"

// std header
#include <list>
#include <vector>

namespace ot {

	class OT_GUI_API_EXPORT ExtendedProjectInformation : public ProjectInformation {
		OT_DECL_DEFCOPY(ExtendedProjectInformation)
		OT_DECL_DEFMOVE(ExtendedProjectInformation)
	public:
		ExtendedProjectInformation();
		ExtendedProjectInformation(const ProjectInformation& _basicInfo);
		virtual ~ExtendedProjectInformation() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void setImageData(const std::vector<char>& _data, ImageFileFormat _format) { m_imageData = _data; m_imageFormat = _format; };
		void setImageData(std::vector<char>&& _data, ImageFileFormat _format) { m_imageData = std::move(_data); m_imageFormat = _format; };
		const std::vector<char>& getImageData() const { return m_imageData; };
		ImageFileFormat getImageFormat() const { return m_imageFormat; };

		void setDescription(const std::string& _description) { m_description = _description; };
		void setDescription(std::string&& _description) { m_description = std::move(_description); };
		const std::string& getDescription() const { return m_description; };

		void setDescriptionSyntax(DocumentSyntax _syntax) { m_descriptionSyntax = _syntax; };
		DocumentSyntax getDescriptionSyntax() const { return m_descriptionSyntax; };

	private:
		std::vector<char> m_imageData;
		ImageFileFormat m_imageFormat;

		DocumentSyntax m_descriptionSyntax;
		std::string m_description;
	};

}
