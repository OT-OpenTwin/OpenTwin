import './DashboardPage.scss';
import React, { useState, useEffect } from "react";
import Navbar from "../../components/Navbar/Navbar";
import DataProvider from './DataProvider';
import GlobalServices from './GlobalServices';
import LocalServiceModal from './LocalServiceModal';

const Dashboard = () => {
    const [loading, setLoading] = useState(true);
    const [localServiceModalIsOpen, setLocalServiceModalIsOpen] = useState(false);
    const [modalContent, setModalContent] = useState(null);
    const [serviceDetails, setServiceDetails] = useState(null);

    useEffect(() => {
        const fetchData = async () => {
            await DataProvider.initialize();
            setLoading(false);
            console.log("Data: ", DataProvider["Global.CPU.Load"])
        };

        fetchData();
    }, []);

    const openLocalServiceModal = async (url) => {
        console.log("openLocalServiceModal called with URL:", url);
        let localServiceData = await DataProvider.getInformation(url);
        setModalContent(localServiceData);
        setLocalServiceModalIsOpen(true);
    };

    const closeLocalServiceModal = () => {
        setLocalServiceModalIsOpen(false);
        setModalContent(null);
        setServiceDetails(null);
    };

    const fetchServiceDetails = async (url) => {
        const details = await DataProvider.getInformation(url);
        setServiceDetails(details);
    };

    if (loading) {
        return <div>Loading...</div>;
    }

    return (
        < div className="dashboardPage" >
            <div className="listContainer container">
                <Navbar />
                <h2>Dashboard</h2>
                <div className="dashboardContainer">
                    <GlobalServices
                        dataProvider={DataProvider}
                        openLocalServiceModal={openLocalServiceModal}
                    />
                </div>
            </div>

            <LocalServiceModal
                isOpen={localServiceModalIsOpen}
                onRequestClose={closeLocalServiceModal}
                modalContent={modalContent}
                fetchServiceDetails={fetchServiceDetails}
                serviceDetails={serviceDetails}
            />
        </div >
    );
};

export default Dashboard;
