/**
 * DashboardPage component manages the display and interaction with global services.
 * It handles data fetching, modal interactions, and renders the dashboard interface.
 * 
 * @component
 * @returns {JSX.Element} - The rendered dashboard page.
 * @author P. Barthel
 */

import './DashboardPage.scss';
import React, { useState, useEffect } from "react";
import Navbar from "../../components/Navbar/Navbar";
import DataProvider from './DataProvider';
import GlobalServices from './GlobalServices';
import LocalServiceModal from './LocalServiceModal';

const Dashboard = () => {
    const [loading, setLoading] = useState(true); // Tracks loading state
    const [localServiceModalIsOpen, setLocalServiceModalIsOpen] = useState(false); // Modal open state
    const [modalContent, setModalContent] = useState(null); // Modal content
    const [serviceDetails, setServiceDetails] = useState(null); // Service details

    // useEffect to fetch data on component mount
    useEffect(() => {
        const fetchData = async () => {
            await DataProvider.initialize(); // Initialize data from provider
            setLoading(false); // Stop loading
            console.log("Data: ", DataProvider["Global.CPU.Load"]);
        };

        fetchData();
    }, []);

    // Function to open local service modal
    const openLocalServiceModal = async (url) => {
        console.log("openLocalServiceModal called with URL:", url);
        let localServiceData = await DataProvider.getInformation(url); // Get local service data
        setModalContent(localServiceData); // Set modal content
        setLocalServiceModalIsOpen(true); // Open modal
    };

    // Function to close local service modal
    const closeLocalServiceModal = () => {
        setLocalServiceModalIsOpen(false); // Close modal
        setModalContent(null); // Clear modal content
        setServiceDetails(null); // Clear service details
    };

    // Function to fetch service details
    const fetchServiceDetails = async (url) => {
        const details = await DataProvider.getInformation(url);
        setServiceDetails(details);
    };

    if (loading) {
        return <div>Loading...</div>; // Display loading state
    }

    // Render dashboard page content
    return (
        <div className="dashboardPage">
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
        </div>
    );
};

export default Dashboard;
