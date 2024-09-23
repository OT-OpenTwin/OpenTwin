import authRequest from "../FesRequest/authRequest";
import msRequest from "../FesRequest/msRequest";

class ServiceFactory {
    static createService(type, serviceURL) {
        switch (type) {
            case 'GetGlobalSessionService':
                return this.createGetGlobalSessionService(serviceURL);
            case 'GetInformationService':
                return this.createGetInformationService(serviceURL);
            default:
                throw new Error(`Unknown service type: ${type}`);
        }
    }

    static createGetGlobalSessionService(serviceURL) {
        const data = {
            action: "Get.GlobalServicesURL",
            LoggedInUsername: "admin",
            LoggedInUserPassword: "admin",
            "Service.URL": serviceURL
        };

        const authServiceAddress = `https://${serviceURL}/execute-one-way-tls`;
        console.log("GSS URL: ", authServiceAddress);
        return msRequest(authServiceAddress, data);
    }

    static createGetInformationService(serviceURL) {
        const data = {
            action: "System.GetInformation",
            LoggedInUsername: "admin",
            LoggedInUserPassword: "admin",
            "Service.URL": serviceURL
        };

        return authRequest(data);
    }
}

export default ServiceFactory;
