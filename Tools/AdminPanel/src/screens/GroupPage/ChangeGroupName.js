import "./style/ChangeGroupName.scss";
import { useRef, useState, useContext } from "react";
import { useTranslation } from "react-i18next";
import { Link, useNavigate } from "react-router-dom";

import { Formik, Field, Form, ErrorMessage } from "formik";
import { UserContext } from "../../components/context/userDetails";
import { changeGroupNameValidationSchema } from "./validations/validation-schema";

import Sidebar from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import { Alert, TextField, Button } from "@mui/material";
import SendIcon from "@mui/icons-material/Send";

import ChangeGroupName from "../../services/api/AuthorisationService/GroupActions/ChangeGroupName";

const ChangeGroupData = () => {
  const { t } = useTranslation();
  const navigate = useNavigate();

  const selectedGroup = useContext(UserContext);

  const oldGroupNameInputRef = useRef();
  const newGroupNameInputRef = useRef();
  const [infoMessage, setInfoMessage] = useState("");

  const initialValues = {
    oldGroup: selectedGroup.userDetails,
    newGroup: "",
  };

  const SendGroupNameChangeRequest = (values, props) => {
    const enteredOldGroupName = oldGroupNameInputRef.current.value;
    const enteredNewGroupName = newGroupNameInputRef.current.value;

    ChangeGroupName({
      enteredOldGroupName,
      enteredNewGroupName,
    }).then((response) => {
      if (response.successful === true) {
        setInfoMessage("Group name changed.");
      } else {
        setInfoMessage(response.description);
      }
    });
    console.log(values);
    console.log(props);
    setTimeout(() => {
      props.resetForm();
      props.setSubmitting(false);
      navigate("/groups");
    }, 2000);
  };

  return (
    <div className="changeGroupName">
      <Sidebar items="group" />
      <div className="groupContainer">
        <Navbar />
        <br />
        <br />
        <br />
        <Formik
          initialValues={initialValues}
          validationSchema={changeGroupNameValidationSchema}
          onSubmit={SendGroupNameChangeRequest}
        >
          {(props) => (
            <Form>
              <div className="top">
                <div className="left">
                  <h1 className="itemTitle">
                    {t("groupPage:changeGroupNameTitle")}
                  </h1>
                  <div className="item">
                    <div className="details">
                      <div> {t("groupPage:oldGroupName")}</div>
                      <Field
                        as={TextField}
                        size="small"
                        variant="standard"
                        type="text"
                        name="oldGroup"
                        inputRef={oldGroupNameInputRef}
                        helperText={<ErrorMessage name="oldGroup" />}
                        InputProps={{
                          readOnly: true,
                        }}
                      />
                      <div> {t("groupPage:newGroupName")}</div>
                      <Field
                        as={TextField}
                        size="small"
                        variant="standard"
                        type="text"
                        name="newGroup"
                        inputRef={newGroupNameInputRef}
                        helperText={<ErrorMessage name="newGroup" />}
                      />
                      <div />
                      <br />
                      <Button
                        variant="contained"
                        endIcon={<SendIcon />}
                        disabled={props.isSubmitting}
                        type="submit"
                      >
                        {props.isSubmitting
                          ? "Changing ..."
                          : t("groupPage:changeButton")}
                      </Button>{" "}
                      {!props.isSubmitting && (
                        <Link to="/groups" style={{ textDecoration: "none" }}>
                          <Button variant="outlined" type="submit">
                            {t("groupPage:bckButton")}
                          </Button>
                        </Link>
                      )}
                      <br />
                      <br />
                      {infoMessage &&
                        (props.isSubmitting ? (
                          <Alert severity="info">{infoMessage}</Alert>
                        ) : (
                          ""
                        ))}
                    </div>
                  </div>
                </div>
              </div>
            </Form>
          )}
        </Formik>
      </div>
    </div>
  );
};

export default ChangeGroupData;
