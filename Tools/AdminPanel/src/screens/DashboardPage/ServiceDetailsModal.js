import React from 'react';
import Modal from 'react-modal';
import './ServiceDetailsModal.scss';

Modal.setAppElement('#root');

const ServiceDetailsModal = ({ isOpen, onRequestClose, serviceDetails }) => {
    return (
        <Modal
            isOpen={isOpen}
            onRequestClose={onRequestClose}
            contentLabel="Service Details"
            overlayClassName="modal-overlay"
            className="modal-content"
        >
            <h2 className='modal-header'>Service Details</h2>
            <button className='modal-close-button' onClick={onRequestClose}>Close</button>
            {serviceDetails && (
                <div className='service-details-info'>
                    <p>Global CPU Load: {parseFloat(serviceDetails["Global.CPU.Load"]).toFixed(2)}%</p>
                    <p>Global Memory Load: {parseFloat(serviceDetails["Global.Memory.Load"]).toFixed(2)}%</p>
                    <p>Process CPU Load: {parseFloat(serviceDetails["Process.CPU.Load"]).toFixed(2)}%</p>
                    <p>Process Memory Load: {parseFloat(serviceDetails["Process.Memory.Load"]).toFixed(2)}%</p>
                    <p>Service Type: {serviceDetails["Service.Type"]}</p>
                    <p>Session ID: {serviceDetails["Session.ID"]}</p>
                    <p>Session Type: {serviceDetails["Session.Type"]}</p>
                </div>
            )}
        </Modal>
    );
};

export default ServiceDetailsModal;
