//! @file StartupDispatcher.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"

// GDS header
#include "ServiceInformation.h"

// std header
#include <list>
#include <mutex>

namespace std { class thread; }

//! @brief The StartupDispatcher is responsible for managing service start requests.
//! It collects requests and starts a worker thread to process them.
class StartupDispatcher {
	OT_DECL_NOCOPY(StartupDispatcher)
	OT_DECL_NOMOVE(StartupDispatcher)
public:
	StartupDispatcher();
	virtual ~StartupDispatcher();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Request management

	//! @brief Add a service start request to the queue.
	//! The request will be processed by the worker thread.
	//! @param _info Information about the service to start.
	void addRequest(ServiceInformation&& _info);

	//! @brief Add multiple service start requests to the queue.
	//! The requests will be processed by the worker thread.
	//! @param _info List of service information objects to start.
	void addRequest(std::list<ServiceInformation>&& _info);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private methods

private:
	//! @brief Run the worker thread if it is not already running.
	void run(void);
	
	//! @brief Stop the worker thread and wait for it to finish.
	void stop(void);

	//! @brief The worker function that processes service start requests.
	void workerFunction(void);

	//! @brief Will notify the session service about a failed service start request.
	//! @warning It is assumed that the mutex is locked when calling this method.
	//! @param _serviceInfo Information about the service that failed to start.
	void serviceStartRequestFailed(const ServiceInformation& _serviceInfo);

	std::list<ServiceInformation> m_requestedServices; //! @brief List of service start requests that are waiting to be processed.
	std::mutex                    m_mutex;             //! @brief Mutex to protect access to the request list.
	std::thread*                  m_workerThread;      //! @brief The worker thread that processes service start requests.
	std::atomic_bool              m_isStopping;        //! @brief Flag indicating whether the worker thread is stopping.
};