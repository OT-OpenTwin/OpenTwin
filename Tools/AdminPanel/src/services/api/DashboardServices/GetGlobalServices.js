import ServiceFactory from './ServiceFactory';

export function GetGlobalSessionService(serviceURL) {
    return ServiceFactory.createService('GetGlobalSessionService', serviceURL);
}

export default GetGlobalSessionService;
