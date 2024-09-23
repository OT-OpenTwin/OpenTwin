import React, { useState } from 'react';
import Modal from 'react-modal';
import ServiceDetailsModal from './ServiceDetailsModal';
import './LocalServiceModal.scss';

Modal.setAppElement('#root');

const LocalSessionServiceModal = ({ isOpen, onRequestClose, modalContent, fetchServiceDetails, serviceDetails }) => {
    const [detailsModalIsOpen, setDetailsModalIsOpen] = useState(false);
    const [expandedSession, setExpandedSession] = useState(null);
    const [filterText, setFilterText] = useState('');
    const [sortAscending, setSortAscending] = useState(true);

    const openDetailsModal = async (url) => {
        await fetchServiceDetails(url);
        setDetailsModalIsOpen(true);
    };

    const closeDetailsModal = () => {
        setDetailsModalIsOpen(false);
    };

    const toggleSession = (sessionId) => {
        setExpandedSession(expandedSession === sessionId ? null : sessionId);
    };

    const handleFilterChange = (e) => {
        setFilterText(e.target.value);
    };

    const handleSortToggle = () => {
        setSortAscending(!sortAscending);
    };

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
                <h2 className='modal-header'>Local Directory Service Details</h2>
                <button className='modal-close-button' onClick={onRequestClose}>Close</button>
                {modalContent ? (
                    <>
                        <div className="usage usage-info">
                            <p>Global CPU usage: {parseFloat(modalContent["Global.CPU.Load"]).toFixed(2)}%</p>
                            <p>Global Memory usage: {parseFloat(modalContent["Global.Memory.Load"]).toFixed(2)}%</p>
                            <p>Process CPU usage: {parseFloat(modalContent["Process.CPU.Load"]).toFixed(2)}%</p>
                            <p>Process Memory usage: {parseFloat(modalContent["Process.Memory.Load"]).toFixed(2)}%</p>
                            <h5>Session Services:</h5>
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
                                            <p>No. of active Services: {session["Session.Services"].length}</p>
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
