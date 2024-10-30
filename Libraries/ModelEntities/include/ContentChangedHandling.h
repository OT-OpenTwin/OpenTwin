#pragma once

namespace ot
{
	//! @brief Defines how the model service shall deal with a content changed notification from the ui. 
	//! Options are to save the changes and notify the owner of the entity, letting the owner handle everything or simply saving the changes without notification
	enum class ContentChangedHandling : int32_t
	{
		NotifyOwner = 0,
		OwnerHandles = 1,
		SimpleSave = 2
	};
}
