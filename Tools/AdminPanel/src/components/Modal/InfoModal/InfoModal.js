import React from "react";

import {
  Dialog,
  DialogContent,
  DialogContentText,
  DialogTitle,
  Button,
} from "@mui/material";

const InfoModal = (props) => {
  return (
    <div>
      <Dialog
        open={props.open}
        onClose={props.handleClose}
        aria-labelledby="alert-dialog-title"
        aria-describedby="alert-dialog-description"
      >
        <DialogTitle id="alert-dialog-title">{props.dialogTitle}</DialogTitle>
        <DialogContent>
          <DialogContentText id="alert-dialog-description">
            {props.dialogContent}
          </DialogContentText>
        </DialogContent>
        <Button onClick={props.handleClose} autoFocus>
          {props.dialogButton}
        </Button>
        <br />
      </Dialog>
    </div>
  );
};

export default InfoModal;
