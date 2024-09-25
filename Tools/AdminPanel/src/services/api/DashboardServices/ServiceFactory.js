/**
 * ServiceFactory class provides a factory method to create service requests
 * based on the service type and URL. It supports creating service requests
 * for global session retrieval and system information retrieval.
 * 
 * @author P. Barthel
 */

import authRequest from "../FesRequest/authRequest";
import msRequest from "../FesRequest/msRequest";

class ServiceFactory {
    /**
     * Creates a service based on the provided type and service URL.
     * 
     * @param {string} type - The type of service to create (e.g., 'GetGlobalSessionService' or 'GetInformationService').
     * @param {string} serviceURL - The URL of the service to which the request will be sent.
     * @returns {Promise} - A promise resolving to the result of the service request.
     * @throws {Error} - If the service type is unknown.
     */
    static createService(type, serviceURL) {
        switch (type) {
            // Create a request for getting a global session service.
            case 'GetGlobalSessionService':
                return this.createGetGlobalSessionService(serviceURL);

            // Create a request for getting system information.
            case 'GetInformationService':
                return this.createGetInformationService(serviceURL);

            // Throw an error if an unknown service type is requested.
            default:
                throw new Error(`Unknown service type: ${type}`);
        }
    }

    /**
     * Creates a request for the global session service using msRequest.
     * This sends a request to the provided service URL using one-way TLS.
     * 
     * @param {string} serviceURL - The URL of the global session service.
     * @returns {Promise} - A promise resolving to the result of the msRequest.
     */
    static createGetGlobalSessionService(serviceURL) {
        // Request data to get the global services URL
        const data = {
            action: "Get.GlobalServicesURL",
            LoggedInUsername: "admin", // Username for authentication (hardcoded).
            LoggedInUserPassword: "admin", // Password for authentication (hardcoded).
            "Service.URL": serviceURL // The URL of the service.
        };

        // The address to send the service request using one-way TLS.
        const authServiceAddress = `https://${serviceURL}/execute-one-way-tls`;
        console.log("GSS URL: ", authServiceAddress);

        // Send the request using msRequest and return the result.
        return msRequest(authServiceAddress, data);
    }

    /**
     * Creates a request for system information using authRequest.
     * 
     * @param {string} serviceURL - The URL of the information service.
     * @returns {Promise} - A promise resolving to the result of the authRequest.
     */
    static createGetInformationService(serviceURL) {
        // Request data to get system information
        const data = {
            action: "System.GetInformation",
            LoggedInUsername: "admin", // Username for authentication (hardcoded).
            LoggedInUserPassword: "admin", // Password for authentication (hardcoded).
            "Service.URL": serviceURL // The URL of the service.
        };

        // Send the request using authRequest and return the result.
        return authRequest(data);
    }
}

export default ServiceFactory;
