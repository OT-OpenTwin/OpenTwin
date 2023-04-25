import "./style/CreateNewGroup.scss";
import { useRef, useState } from "react";
import { useTranslation } from "react-i18next";
import { Link } from "react-router-dom";

import { Formik, Field, Form, ErrorMessage } from "formik";
import { addGroupInitialValues } from "./validations/initial-values";
import { addGroupValidationSchema } from "./validations/validation-schema";

import Sidebar from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import { Alert, TextField, Button } from "@mui/material";
import SendIcon from "@mui/icons-material/Send";

import AddNewGroup from "../../services/api/AuthorisationService/GroupActions/AddNewGroup";

const CreateNewGroup = ({ title }) => {
  const { t } = useTranslation();

  const groupNameInputRef = useRef();
  const [createdMessages, setCreatedMessages] = useState("");

  const SendCreateGroupRequest = (values, props) => {
    const enteredGroupName = groupNameInputRef.current.value;

    AddNewGroup({ enteredGroupName }).then((data) => {
      if (data.groupName) {
        setCreatedMessages("Group " + data.groupName + " is created");
      } else {
        setCreatedMessages(enteredGroupName + " is already exists.");
      }
    });
    console.log(values);
    console.log(props);
    setTimeout(() => {
      props.resetForm();
      props.setSubmitting(false);
    }, 2000);
  };

  return (
    <div className="newGroup">
      <Sidebar items="group" />
      <div className="newContainer">
        <br />
        <br />
        <br />
        <Navbar />
        <div className="top">
          <h1>{title}</h1>
        </div>
        <div className="bottom">
          <Formik
            initialValues={addGroupInitialValues}
            validationSchema={addGroupValidationSchema}
            onSubmit={SendCreateGroupRequest}
          >
            {(props) => (
              <Form>
                <div className="formInput">
                  <label>{t("groupPage:groupName")}</label>
                  <Field
                    as={TextField}
                    size="small"
                    variant="standard"
                    type="text"
                    name="groupName"
                    inputRef={groupNameInputRef}
                    helperText={<ErrorMessage name="groupName" />}
                  />
                  <br />
                  <br />
                  <Button
                    variant="contained"
                    endIcon={<SendIcon />}
                    disabled={props.isSubmitting}
                    type="submit"
                  >
                    {props.isSubmitting
                      ? "Creating ..."
                      : t("groupPage:button")}
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
                  {createdMessages &&
                    (props.isSubmitting ? (
                      <Alert severity="info">{createdMessages}</Alert>
                    ) : (
                      ""
                    ))}
                </div>
              </Form>
            )}
          </Formik>
        </div>
      </div>
    </div>
  );
};

export default CreateNewGroup;
