/**
 * LocalSessionServiceModal component manages the modal that displays
 * detailed information about local session services.
 * 
 * @component
 * @param {object} props - The component props.
 * @param {boolean} props.isOpen - Determines if the modal is open.
 * @param {function} props.onRequestClose - Function to close the modal.
 * @param {object} props.modalContent - The content to display in the modal.
 * @param {function} props.fetchServiceDetails - Function to fetch detailed service information.
 * @param {object} props.serviceDetails - The details of a specific service to display in a sub-modal.
 * @returns {JSX.Element} - The rendered LocalSessionServiceModal component.
 * @author P. Barthel
 */

import React, { useState } from 'react';
import Modal from 'react-modal';
import ServiceDetailsModal from './ServiceDetailsModal';
import './LocalServiceModal.scss';

Modal.setAppElement('#root');

const LocalSessionServiceModal = ({ isOpen, onRequestClose, modalContent, fetchServiceDetails, serviceDetails }) => {
    const [detailsModalIsOpen, setDetailsModalIsOpen] = useState(false); // State to manage sub-modal for service details
    const [expandedSession, setExpandedSession] = useState(null); // State to track which session is expanded
    const [filterText, setFilterText] = useState(''); // State to manage session filtering
    const [sortAscending, setSortAscending] = useState(true); // State to toggle sort order

    // Open the details modal and fetch additional service information
    const openDetailsModal = async (url) => {
        await fetchServiceDetails(url);
        setDetailsModalIsOpen(true);
    };

    // Close the service details modal
    const closeDetailsModal = () => {
        setDetailsModalIsOpen(false);
    };

    // Toggle session expansion to show or hide details
    const toggleSession = (sessionId) => {
        setExpandedSession(expandedSession === sessionId ? null : sessionId);
    };

    // Handle filter text input change
    const handleFilterChange = (e) => {
        setFilterText(e.target.value);
    };

    // Toggle sort order between ascending and descending
    const handleSortToggle = () => {
        setSortAscending(!sortAscending);
    };

    // Filter and sort sessions based on user input
    const filteredSessions = modalContent && modalContent["Session.List"]
        ? modalContent["Session.List"]
            .filter(session => session["Session.ID"].toLowerCase().includes(filterText.toLowerCase()))
            .sort((a, b) => sortAscending ? a["Session.ID"].localeCompare(b["Session.ID"]) : b["Session.ID"].localeCompare(a["Session.ID"]))
        : [];

    return (
        <>
            <Modal
                isOpen={isOpen}
                onRequestClose={onRequestClose}
                contentLabel="Local Session Service Details"
                overlayClassName="modal-overlay"
                className="modal-content"
            >
                <h2 className='modal-header'>{modalContent && modalContent["Service.Type"] ? modalContent["Service.Type"] + " Details" : ""}</h2>
                <button className='modal-close-button' onClick={onRequestClose}>Close</button>
                {modalContent ? (
                    <>
                        <div className="usage usage-info">
                            <p>Global CPU usage: {parseFloat(modalContent["Global.CPU.Load"]).toFixed(2)}%</p>
                            <p>Global Memory usage: {parseFloat(modalContent["Global.Memory.Load"]).toFixed(2)}%</p>
                            <p>Process CPU usage: {parseFloat(modalContent["Process.CPU.Load"]).toFixed(2)}%</p>
                            <p>Process Memory usage: {parseFloat(modalContent["Process.Memory.Load"]).toFixed(2)}%</p>
                        </div>
                        {modalContent["Services.Supported"] && (
                            <ul className='supported-services'>
                                <h3>Supported Services:</h3>
                                {modalContent["Services.Supported"].map((service, index) => (
                                    <li key={index}>
                                        {service}
                                    </li>
                                ))}
                            </ul>
                        )}
                        <div className='session-list-controls'>
                            <input
                                type="text"
                                placeholder="Filter by Session ID"
                                value={filterText}
                                onChange={handleFilterChange}
                            />
                            <button onClick={handleSortToggle}>
                                Sort {sortAscending ? 'Descending' : 'Ascending'}
                            </button>
                        </div>
                        <ul className='session-list'>
                            <h3>Session List:</h3>
                            {filteredSessions.map((session, index) => (
                                <li key={index}>
                                    <h4 onClick={() => toggleSession(session["Session.ID"])}>Session ID: {session["Session.ID"]}</h4>
                                    {expandedSession === session["Session.ID"] && (
                                        <div>
                                            <p>No. of active Services: {session["Session.Services"] ? session["Session.Services"].length : 0}</p>
                                            <ul>
                                                {session["Session.Services"].map((service, idx) => (
                                                    <li className='session-service' key={idx}>
                                                        {service["Service.URL"]} - {service["Service.Type"]}
                                                        <button className='service-details-button' onClick={() => openDetailsModal(service["Service.URL"])}>Get Service Details</button>
                                                    </li>
                                                ))}
                                            </ul>
                                        </div>
                                    )}
                                </li>
                            ))}
                        </ul>
                    </>
                ) : (
                    <p>Loading...</p>
                )}
            </Modal>

            <ServiceDetailsModal
                isOpen={detailsModalIsOpen}
                onRequestClose={closeDetailsModal}
                serviceDetails={serviceDetails}
            />
        </>
    );
};

export default LocalSessionServiceModal;
