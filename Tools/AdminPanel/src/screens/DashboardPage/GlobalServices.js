import './GlobalServices.scss';
import SpeedIcon from '@mui/icons-material/Speed';
import MemoryIcon from '@mui/icons-material/Memory';
import FormatListNumberedIcon from '@mui/icons-material/FormatListNumbered';

const generateDummyLocalServices = (count) => {
    const services = [];
    for (let i = 1; i <= count; i++) {
        services.push({ URL: `http://localhost:300${i}` });
    }
    return services;
};

const GlobalService = ({ serviceType, serviceData, openLocalServiceModal }) => (
    <div className="grid-item">
        <h3>Global {serviceType} Service</h3>
        <div className="grid-container">
            <div className="grid-item usage-icon">
                <SpeedIcon style={{ fontSize: '6em' }} />
            </div>
            <div className="grid-item">
                Global CPU usage: {parseFloat(serviceData["Global.CPU.Load"]).toFixed(2)}%
            </div>
            <div className="grid-item">
                GSS-Process CPU usage: {parseFloat(serviceData["Process.CPU.Load"]).toFixed(2)}%
            </div>
        </div>
        <div className="grid-container" style={{ marginTop: '1.5em' }}>
            <div className="grid-item usage-icon">
                <MemoryIcon style={{ fontSize: '6em' }} />
            </div>
            <div className="grid-item">
                Global Memory usage: {parseFloat(serviceData["Global.Memory.Load"]).toFixed(2)}%
            </div>
            <div className="grid-item">
                GSS-Process Memory usage: {parseFloat(serviceData["Process.Memory.Load"]).toFixed(2)}%
            </div>
        </div>
        <ul style={{ marginTop: '1.5em' }}>
            <h3>Local {serviceType} Services:</h3>
            <div className="grid-container">
                <div className="grid-item" style={{ display: 'flex', alignItems: 'center', justifyContent: 'center' }}>
                    <FormatListNumberedIcon style={{ fontSize: '1.5em' }} />
                </div>
                <div className="grid-item">
                    No. of active Services: <b>{serviceData["LocalServices"].length}</b>
                </div>
                {serviceData["LocalServices"].map((service, index) => (
                    <li key={index} className="service-item">
                        <div className="grid-item">
                            {service.URL}
                        </div>
                        <button className="service-details-button" style={{ marginLeft: '2em' }} onClick={() => openLocalServiceModal(service.URL)}>Show Details</button>
                    </li>
                ))}
                {/* {generateDummyLocalServices(4).map((service, index) => (
                    <li key={index} className="service-item">
                        <div className="grid-item">
                            {service.URL}
                        </div>
                        <button className="service-details-button" style={{ marginLeft: '2em' }} onClick={() => openLocalServiceModal(service.URL)}>Show Details</button>
                    </li>
                ))} */}
            </div>
        </ul>
    </div>
);

const GlobalServices = ({ dataProvider, openLocalServiceModal }) => (
    <div className="grid-container">
        <GlobalService
            serviceType="Session"
            serviceData={{
                "Global.CPU.Load": dataProvider.globalSessionService["Global.CPU.Load"],
                "Process.CPU.Load": dataProvider.globalSessionService["Process.CPU.Load"],
                "Global.Memory.Load": dataProvider.globalSessionService["Global.Memory.Load"],
                "Process.Memory.Load": dataProvider.globalSessionService["Process.Memory.Load"],
                "LocalServices": dataProvider.globalSessionService["LocalSessionServices"],
            }}
            openLocalServiceModal={openLocalServiceModal}
        />
        <GlobalService
            serviceType="Directory"
            serviceData={{
                "Global.CPU.Load": dataProvider.globalDirectoryService["Global.CPU.Load"],
                "Process.CPU.Load": dataProvider.globalDirectoryService["Process.CPU.Load"],
                "Global.Memory.Load": dataProvider.globalDirectoryService["Global.Memory.Load"],
                "Process.Memory.Load": dataProvider.globalDirectoryService["Process.Memory.Load"],
                "LocalServices": dataProvider.globalDirectoryService["LocalDirectoryServices"],
            }}
            openLocalServiceModal={openLocalServiceModal}
        />
    </div>
);

export default GlobalServices;
