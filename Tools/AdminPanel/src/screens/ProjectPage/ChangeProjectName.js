import "./style/ChangeProjectName.scss";
import { useState, useRef, useContext } from "react";
import { useTranslation } from "react-i18next";
import { Link, useNavigate } from "react-router-dom";

import { Formik, Field, Form, ErrorMessage } from "formik";
import { UserContext } from "../../components/context/userDetails";
import { changeProjectNameValidationSchema } from "./validations/validation-schema";

import { Sidebar } from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import { Alert, TextField, Button } from "@mui/material";
import SendIcon from "@mui/icons-material/Send";

import ChangeProjectName from "../../services/api/AuthorisationService/ProjectActions/ChangeProjectName";

const ChangeProjectData = () => {
  const { t } = useTranslation();
  const navigate = useNavigate();

  const selectedProject = useContext(UserContext);

  const oldProjectNameInputRef = useRef();
  const newProjectNameInputRef = useRef();
  const [infoMessage, setInfoMessage] = useState("");

  const initialValues = {
    oldProject: selectedProject.userDetails,
    newProject: "",
  };

  const SendProjectNameChangeRequest = (values, props) => {
    const enteredOldProjectName = oldProjectNameInputRef.current.value;
    const enteredNewProjectName = newProjectNameInputRef.current.value;
    ChangeProjectName({
      enteredOldProjectName,
      enteredNewProjectName,
    }).then((response) => {
      console.log("res", response);
      if (response.name !== undefined) {
        setInfoMessage("Project name changed.");
      } else {
        setInfoMessage(response.description);
      }
    });
    console.log(values);
    console.log(props);
    setTimeout(() => {
      props.resetForm();
      props.setSubmitting(false);
      navigate("/projects");
    }, 2000);
  };

  return (
    <div className="changeProject">
      <Sidebar items="project" />
      <div className="singleContainer">
        <Navbar />
        <br />
        <br />
        <br />
        <Formik
          initialValues={initialValues}
          validationSchema={changeProjectNameValidationSchema}
          onSubmit={SendProjectNameChangeRequest}
        >
          {(props) => (
            <Form>
              <div className="top">
                <div className="left">
                  <h1 className="itemTitle">
                    {t("projectPage:changeprojectNameTitle")}
                  </h1>
                  <br />
                  <div className="item">
                    <div className="details">
                      <div> {t("projectPage:oldProjectName")}</div>
                      <Field
                        as={TextField}
                        size="small"
                        variant="standard"
                        type="text"
                        name="oldProject"
                        inputRef={oldProjectNameInputRef}
                        helperText={<ErrorMessage name="oldProject" />}
                        InputProps={{
                          readOnly: true,
                        }}
                      />
                      <div> {t("projectPage:newProjectName")}</div>
                      <Field
                        as={TextField}
                        size="small"
                        variant="standard"
                        type="text"
                        name="newProject"
                        inputRef={newProjectNameInputRef}
                        helperText={<ErrorMessage name="newProject" />}
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
                          ? "Changing ..."
                          : t("projectPage:changeButton")}
                      </Button>{" "}
                      {!props.isSubmitting && (
                        <Link to="/projects" style={{ textDecoration: "none" }}>
                          <Button variant="outlined" type="submit">
                            {t("projectPage:bckButton")}
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

export default ChangeProjectData;
