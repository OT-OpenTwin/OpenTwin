//! @file Transform.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	//! \brief General item transformation.
	class OT_GUI_API_EXPORT Transform : public Serializable {
	public:
		//! \brief Flip state.
		enum FlipState {
			NoFlip           = 0x00, //! \brief Item is not flipped.
			FlipVertically   = 0x01, //! \brief Item is flipped vertically.
			FlipHorizontally = 0x02 //! \brief Item is flipped horizontally.
		};
		typedef Flags<FlipState> FlipStateFlags; //! \brief Flip state flags.

		//! \brief Default constructor.
		//! No rotation angle and no flip.
		Transform();
		Transform(double _rotationAngle, const FlipStateFlags& _flipState = FlipStateFlags(FlipState::NoFlip));
		Transform(const Transform& _other);
		virtual ~Transform();

		Transform& operator = (const Transform& _other);

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! \brief Set item rotation.
		//! The angle must be between 0 and 359 degrees.
		void setRotation(double _angle) { m_rotation = _angle; };

		//! \brief Item rotation.
		//! The angle must be between 0 and 359 degrees.
		double getRotation(void) const { return m_rotation; };

		void setFlipState(FlipState _state, bool _active = true) { m_flipState.setFlag(_state, _active); };
		void setFlipStateFlags(const FlipStateFlags& _flags) { m_flipState = _flags; };
		const FlipStateFlags& getFlipStateFlags(void) const { return m_flipState; };

		//! \brief Returns true if any transformation is set.
		bool hasTransform(void) const { return m_rotation != 0. || m_flipState != FlipState::NoFlip; };

	private:
		double m_rotation; //! \brief Item rotation.
		FlipStateFlags m_flipState; //! \brief Item flip state.
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::Transform::FlipState)