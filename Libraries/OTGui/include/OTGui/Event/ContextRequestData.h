// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Widgets/WidgetViewBase.h"

// std header
#include <map>
#include <string>
#include <functional>

namespace ot {

	class OT_GUI_API_EXPORT ContextRequestData : public BasicEntityInformation
	{
		OT_DECL_DEFCOPY(ContextRequestData)
		OT_DECL_DEFMOVE(ContextRequestData)
	public:
		typedef std::function<ContextRequestData* ()> ContextConstructorFunction;

		friend class Registrar;
		template <typename T> class Registrar
		{
		public:
			Registrar(const std::string& _className)
			{
				ContextRequestData::registerClass(_className, []() -> ContextRequestData* { return new T(); });
			}
		};


		static ContextRequestData* fromJson(const std::string& _jsonString);
		static ContextRequestData* fromJson(const ConstJsonObject& _jsonObject);

		ContextRequestData() = default;
		~ContextRequestData() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* classNameJsonKey() noexcept { return "ClassName"; };
		static constexpr const char* className() noexcept { return "ContextRequestData"; };
		virtual std::string getClassName() const { return ContextRequestData::className(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setPosition(double _x, double _y) { m_pos.set(_x, _y); };
		void setPosition(const Point2DD& _pos) { m_pos = _pos; };
		void setPosition(Point2DD&& _pos) { m_pos = std::move(_pos); };
		const Point2DD& getPosition() const { return m_pos; };

		void setViewType(WidgetViewBase::ViewType _viewType) { m_viewType = _viewType; };
		WidgetViewBase::ViewType getViewType() const { return m_viewType; };

	protected:
		static void registerClass(const std::string& _className, ContextConstructorFunction _constructor);
		
	private:
		static std::map<std::string, ContextConstructorFunction>& getRegisteredClasses();

		Point2DD m_pos;
		WidgetViewBase::ViewType m_viewType{ WidgetViewBase::CustomView };
	};

} // namespace ot