/**
 * DataProvider class acts as a singleton to fetch and manage
 * global and local service data, including CPU and memory usage details.
 * 
 * @singleton
 * @class DataProvider
 * @author P. Barthel
 */

import GetInformation from '../../services/api/DashboardServices/GetInformationService';
import GetGlobalServices from '../../services/api/DashboardServices/GetGlobalServices';

class DataProvider {
    static instance = null; // Singleton instance

    constructor() {
        if (DataProvider.instance) {
            return DataProvider.instance;
        }

        this.globalSessionSericeURL = JSON.parse(sessionStorage.getItem("sessionservice")); // URL from session storage
        this.globalServices = null; // Holds global services
        this.globalSessionService = null; // Global session service data
        this.globalDirectoryService = null; // Global directory service data

        DataProvider.instance = this; // Set singleton instance
    }

    // Retrieve or create instance of DataProvider
    static getInstance() {
        if (!DataProvider.instance) {
            DataProvider.instance = new DataProvider();
        }
        return DataProvider.instance;
    }

    // Initialize global services and session information
    async initialize() {
        try {
            this.globalServices = await GetGlobalServices(this.globalSessionSericeURL); // Get global services
            this.globalSessionService = await GetInformation(this.globalSessionSericeURL); // Get global session service info
            this.globalDirectoryService = await GetInformation("127.0.0.1:9094"); // Get global directory service info
        } catch (error) {
            console.error("Error fetching data:", error); // Log any errors
        }
    }

    // Get information for local session services
    async getInformation(localSessionServiceURL) {
        try {
            const response = await GetInformation(localSessionServiceURL); // Fetch info from service URL
            if (response && response["Session.List"]) {
                response["Session.List"] = response["Session.List"].map(session => {
                    session["Session.Services"] = session["Session.Services"].filter(service => service["Service.Type"] !== "RelayService" && service["Service.Type"] !== "UI");
                    return session;
                });
            }
            return response; // Return filtered response
        } catch (error) {
            console.error("Error fetching data:", error); // Log errors
        }
    }

    // Get local session services
    getLocalSessionServices() {
        return this.globalSessionService ? this.globalSessionService["LocalSessionServices"] : [];
    }

    // Get local directory services
    getLocalDirectoryServices() {
        return this.globalDirectoryService ? this.globalDirectoryService["LocalDirectoryServices"] : [];
    }
}

// Export singleton instance
export default DataProvider.getInstance();
