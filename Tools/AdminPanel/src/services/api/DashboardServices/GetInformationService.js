/**
 * Provides a function to retrieve system information by creating
 * a service request using the ServiceFactory.
 * 
 * @param {string} serviceURL - The URL of the service to retrieve information from.
 * @returns {Promise} - A promise resolving to the result of the GetInformationService request.
 * @author P. Barthel
 */

import ServiceFactory from './ServiceFactory';

/**
 * Retrieves system information from the specified service URL.
 * 
 * @param {string} serviceURL - The URL of the service to retrieve information from.
 * @returns {Promise} - A promise resolving to the result of the GetInformationService request.
 */
export function GetInformation(serviceURL) {
    // Use the ServiceFactory to create and send the 'GetInformationService' request.
    return ServiceFactory.createService('GetInformationService', serviceURL);
}

export default GetInformation;
