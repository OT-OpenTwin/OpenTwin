import React from "react";
import "bootstrap/dist/css/bootstrap.min.css";
import { Button, Modal, ModalFooter, ModalHeader, ModalBody } from "reactstrap";
import { ChangeOwnerModalHeader } from "../../../resources/dataGridSources";
import { DataGrid } from "@mui/x-data-grid";
import { useTranslation } from "react-i18next";

const ChangeOwnerModal = ({ modal, handleToggle, ...props }) => {
  const { t } = useTranslation();
  return (
    <div>
      <Modal
        isOpen={modal}
        toggle={handleToggle}
        size="m"
        backdrop="static"
        centered={true}
      >
        <ModalHeader
          toggle={handleToggle}
          cssModule={{ "modal-title": "w-100 text-center" }}
        >
          {props.modalTitle}
        </ModalHeader>
        <ModalBody
          style={{
            height: "350px",
          }}
        >
          <DataGrid
            checkboxSelection
            rows={props.userList}
            columns={ChangeOwnerModalHeader}
            pageSize={7}
            rowsPerPageOptions={[7]}
            onSelectionModelChange={(id) => {
              props.onUserIdSelect(id);
            }}
          />
        </ModalBody>
        <ModalFooter>
          <Button color="primary" onClick={props.onSaveBtnClick}>
            {t("changeOwnerModal:changeButton")}
          </Button>{" "}
          <Button color="secondary" onClick={handleToggle}>
            {t("changeOwnerModal:cancelButton")}
          </Button>
        </ModalFooter>
      </Modal>
    </div>
  );
};

export default ChangeOwnerModal;
