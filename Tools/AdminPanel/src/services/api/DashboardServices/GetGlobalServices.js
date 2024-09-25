/**
 * Provides a function to retrieve global session service by creating
 * a service request using the ServiceFactory.
 * 
 * @param {string} serviceURL - The URL of the service to retrieve the global session from.
 * @returns {Promise} - A promise resolving to the result of the GetGlobalSessionService request.
 * @author P. Barthel
 */

import ServiceFactory from './ServiceFactory';

/**
 * Retrieves global session service from the specified service URL.
 * 
 * @param {string} serviceURL - The URL of the service to retrieve the session from.
 * @returns {Promise} - A promise resolving to the result of the GetGlobalSessionService request.
 */
export function GetGlobalSessionService(serviceURL) {
    // Use the ServiceFactory to create and send the 'GetGlobalSessionService' request.
    return ServiceFactory.createService('GetGlobalSessionService', serviceURL);
}

export default GetGlobalSessionService;
