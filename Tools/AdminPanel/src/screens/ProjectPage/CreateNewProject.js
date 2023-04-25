import "./style/CreateNewProject.scss";
import { useRef, useState } from "react";
import { useTranslation } from "react-i18next";
import { Link } from "react-router-dom";

import { Formik, Field, Form, ErrorMessage } from "formik";
import { addProjectInitialValues } from "./validations/initial-values";
import { addProjectValidationSchema } from "./validations/validation-schema";

import { Sidebar } from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import { Alert, TextField, Button } from "@mui/material";
import SendIcon from "@mui/icons-material/Send";

import AddNewProject from "../../services/api/AuthorisationService/ProjectActions/AddNewProject";

const CreateNewProject = ({ title }) => {
  const { t } = useTranslation();

  const projectNameInputRef = useRef();
  const [createdMessages, setCreatedMessages] = useState("");

  const SendCreateProjectRequest = (values, props) => {
    const enteredProjectName = projectNameInputRef.current.value;
    AddNewProject({
      enteredProjectName,
    }).then((data) => {
      if (data.name) {
        setCreatedMessages("Project " + data.name + " is created");
      } else {
        setCreatedMessages(enteredProjectName + " is already exists.");
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
    <div className="newProject">
      <Sidebar items="project" />
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
            initialValues={addProjectInitialValues}
            validationSchema={addProjectValidationSchema}
            onSubmit={SendCreateProjectRequest}
          >
            {(props) => (
              <Form>
                <div className="formInput">
                  <label> {t("projectPage:projectName")}</label>
                  <Field
                    as={TextField}
                    size="small"
                    variant="standard"
                    type="text"
                    name="projectName"
                    inputRef={projectNameInputRef}
                    helperText={<ErrorMessage name="projectName" />}
                  />
                  <br />
                  <br />
                  <Button
                    variant="contained"
                    endIcon={<SendIcon />}
                    type="submit"
                    disabled={props.isSubmitting}
                  >
                    {props.isSubmitting
                      ? "Creating ..."
                      : t("projectPage:button")}
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

export default CreateNewProject;
