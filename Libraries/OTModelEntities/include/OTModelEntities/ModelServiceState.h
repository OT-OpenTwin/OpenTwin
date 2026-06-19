// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/JSON/JSON.h"
#include "OTModelEntities/ModelEntitiesAPIExport.h"

namespace ot
{
	class ModelServiceStateRAII;

	class OT_MODELENTITIES_API_EXPORT ModelServiceState
	{
		OT_DECL_NOCOPY(ModelServiceState)
		OT_DECL_NOMOVE(ModelServiceState)
	public:
		enum State
		{
			Uninitialized,         //! @brief The model service is not initialized. No project is open.
			OpeningProject,        //! @brief The model service is currently opening a project.
			Running,               //! @brief The model service is currently running and had a project open.
			BuildingNavigationTree //! @brief The model service is currently building the navigation tree (after opening a project).
		};

		ModelServiceState();
		~ModelServiceState() = default;
		
		void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator);

		bool operator == (State _state) const { return m_state == _state; };
		bool operator != (State _state) const { return m_state != _state; };

		void operator = (State _state) { m_state = _state; };

		OT_DECL_NODISCARD ModelServiceStateRAII setTemporaryState(State _state);

		inline void setState(State _state) { m_state = _state; };
		inline State getState() const { return m_state; };

		inline bool isInitialized() const { return m_state != State::Uninitialized; };
		inline bool isRunning() const { return m_state == State::Running; };
		inline bool isOpeningProject() const { return m_state == State::OpeningProject; };

		//! @brief Set the flag indicating whether a project is currently open.
		inline void setProjectIsOpen(bool _isOpen) { m_projectIsOpen = _isOpen; };

		//! @brief Get the flag indicating whether a project is currently open.
		inline bool getProjectIsOpen() const { return m_projectIsOpen; };

		//! @brief Set the flag indicating whether the model is currently shutting down.
		//! During shutdown, no more modified messages are sent to the viewer, which can be used to avoid unnecessary processing in the viewer during shutdown.
		inline void setIsShuttingDown(bool _isShuttingDown) { m_isShuttingDown = _isShuttingDown; };

		//! @brief Get the flag indicating whether the model is currently shutting down.
		//! During shutdown, no more modified messages are sent to the viewer, which can be used to avoid unnecessary processing in the viewer during shutdown.
		inline bool getIsShuttingDown() const { return m_isShuttingDown; };

	private:
		State m_state = State::Uninitialized;
		bool m_projectIsOpen = false;
		bool m_isShuttingDown = false; //! @brief Flag indicating whether the model is currently shutting down (no more modified messages are sent).
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_MODELENTITIES_API_EXPORT ModelServiceStateRAII
	{
		OT_DECL_NOCOPY(ModelServiceStateRAII)
	public:
		explicit ModelServiceStateRAII(ModelServiceState& _modelServiceState, ModelServiceState::State _stateToSet);
		ModelServiceStateRAII(ModelServiceStateRAII&& _other);
		~ModelServiceStateRAII();

	private:
		ModelServiceState* m_modelServiceState;
		ModelServiceState::State m_previousState;
	};

}