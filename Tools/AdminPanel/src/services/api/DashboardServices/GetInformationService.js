import ServiceFactory from './ServiceFactory';

export function GetInformation(serviceURL) {
    return ServiceFactory.createService('GetInformationService', serviceURL);
}

export default GetInformation;
