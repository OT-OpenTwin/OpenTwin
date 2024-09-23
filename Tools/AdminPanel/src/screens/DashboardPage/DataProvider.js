import GetInformation from '../../services/api/DashboardServices/GetInformationService';
import GetGlobalServices from '../../services/api/DashboardServices/GetGlobalServices';

class DataProvider {
    static instance = null;

    constructor() {
        if (DataProvider.instance) {
            return DataProvider.instance;
        }

        this.globalSessionSericeURL = JSON.parse(sessionStorage.getItem("sessionservice"));
        this.globalServices = null;
        this.globalSessionService = null;
        this.globalDirectoryService = null;

        DataProvider.instance = this;
    }

    static getInstance() {
        if (!DataProvider.instance) {
            DataProvider.instance = new DataProvider();
        }
        return DataProvider.instance;
    }

    async initialize() {
        try {
            // Retrieve the URLs of the global services (DB, Auth, GDS)
            this.globalServices = await GetGlobalServices(this.globalSessionSericeURL);

            // Retrieve information about Global Session Service
            this.globalSessionService = await GetInformation(this.globalSessionSericeURL);

            // Retrieve information about Global Directory Service
            this.globalDirectoryService = await GetInformation("127.0.0.1:9094");
        } catch (error) {
            console.error("Error fetching data:", error);
        }
    }

    async getInformation(localSessionServiceURL) {
        try {
            const response = await GetInformation(localSessionServiceURL);
            if (response && response["Session.List"]) {
                response["Session.List"] = response["Session.List"].map(session => {
                    session["Session.Services"] = session["Session.Services"].filter(service => service["Service.Type"] !== "RelayService" && service["Service.Type"] !== "UI");
                    return session;
                });
            }
            return response;
        } catch (error) {
            console.error("Error fetching data:", error);
        }
    }

    getLocalSessionServices() {
        return this.globalSessionService ? this.globalSessionService["LocalSessionServices"] : [];
    }

    getLocalDirectoryServices() {
        return this.globalDirectoryService ? this.globalDirectoryService["LocalDirectoryServices"] : [];
    }
}

export default DataProvider.getInstance();
