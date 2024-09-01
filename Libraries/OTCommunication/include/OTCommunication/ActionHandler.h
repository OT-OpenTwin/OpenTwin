//! @file ActionHandler.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

namespace ot {

	class ActionHandler {
	public:
		ActionHandler() = default;
		ActionHandler(const ActionHandler& _other) = default;
		ActionHandler(ActionHandler&& _other) = default;
		virtual ~ActionHandler() = default;
		ActionHandler& operator = (const ActionHandler& _other) = default;
		ActionHandler& operator = (ActionHandler&& _other) = default;
	};

}