// @otlicense

//! @file DialogCfg.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/Size2D.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT DialogCfg : public Serializable {
		OT_DECL_DEFCOPY(DialogCfg)
		OT_DECL_DEFMOVE(DialogCfg)
	public:
		enum DialogFlag {
			NoFlags          = 0x00, //! @brief No dialog flags
			CancelOnNoChange = 0x01, //! @brief If set the UI will silently close the dialog when pressing confim button and the initial data did not change
			MoveGrabAnywhere = 0x02, //! @brief If set the dialog may be moved around by holding the left mouse button anywhere in the dialog and move.
			RecenterOnF11    = 0x04  //! @brief If set the dialog will move to the center of the screen if the user presses the "F11" key.
		};
		typedef Flags<DialogFlag> DialogFlags;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Static helper

		static std::string toString(DialogFlag _flag);
		static DialogFlag stringToFlag(const std::string& _flag);
		static std::list<std::string> toStringList(DialogFlags _flags);
		static DialogFlags stringListToFlags(const std::list<std::string>& _flags);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		DialogCfg(DialogFlags _flags = DialogFlags(NoFlags));
		DialogCfg(const std::string& _title, DialogFlags _flags = DialogFlags(NoFlags));
		virtual ~DialogCfg() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle(void) const { return m_title; };

		void setFlag(DialogFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setFlags(DialogFlags _flags) { m_flags = _flags; };
		DialogFlags getFlags(void) const { return m_flags; };

		//! @brief Set the initial dialog size.
		//! The initial dialog size will be applied when showing the dialog.
		//! @param _width Width to set. If the width is set to -1, the setting will be ignored.
		//! @param _height Height to set. If the height is set to -1, the setting will be ignored.
		void setInitialSize(int _width, int _height) { this->setInitialSize(ot::Size2D(_width, _height)); };

		//! @brief Set the initial dialog size.
		//! The initial dialog size will be applied when showing the dialog.
		//! @param _size Size to set. If the width or height is set to -1, the corresponding setting will be ignored.
		void setInitialSize(const ot::Size2D& _size) { m_initialSize = _size; };

		//! @brief Get the initial dialog size.
		//! @ref setInitialSize(int _width, int _height)
		const ot::Size2D& getInitialSize(void) const { return m_initialSize; };

		//! @brief Set the minimal dialog size.
		//! @param _width Width to set. If the width is set to -1, the setting will be ignored.
		//! @param _height Height to set. If the height is set to -1, the setting will be ignored.
		void setMinSize(int _width, int _height) { this->setMinSize(ot::Size2D(_width, _height)); };

		//! @brief Set the minimal dialog size.
		//! @param _size Size to set. If the width or height is set to -1, the corresponding setting will be ignored.
		void setMinSize(const ot::Size2D& _size) { m_minSize = _size; };

		//! @brief Get the minimal dialog size.
		//! @ref setMinSize(int _width, int _height)
		const ot::Size2D& getMinSize(void) const { return m_minSize; };

		//! @brief Set the maximum dialog size.
		//! @param _width Width to set. If the width is set to -1, the setting will be ignored.
		//! @param _height Height to set. If the height is set to -1, the setting will be ignored.
		void setMaxSize(int _width, int _height) { this->setMaxSize(ot::Size2D(_width, _height)); };

		//! @brief Set the maximum dialog size.
		//! @param _size Size to set. If the width or height is set to -1, the corresponding setting will be ignored.
		void setMaxSize(const ot::Size2D& _size) { m_maxSize = _size; };

		//! @brief Get the maximum dialog size.
		//! @ref setMinSize(int _width, int _height)
		const ot::Size2D& getMaxSize(void) const { return m_maxSize; };

	private:
		std::string m_name;
		std::string m_title;
		DialogFlags m_flags;
		ot::Size2D m_minSize;
		ot::Size2D m_initialSize;
		ot::Size2D m_maxSize;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::DialogCfg::DialogFlag)