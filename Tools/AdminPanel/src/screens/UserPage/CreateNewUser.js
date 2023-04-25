import "./style/CreateNewUser.scss";
import { useRef, useState } from "react";
import { useTranslation } from "react-i18next";
import { Link } from "react-router-dom";

import { Formik, Field, Form, ErrorMessage } from "formik";
import { addUserInitialValues } from "./validations/initial-values";
import { addUserValidationSchema } from "./validations/validation-schema";

import Sidebar from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import { Alert, TextField, Button } from "@mui/material";
import SendIcon from "@mui/icons-material/Send";

import RegisterNewUser from "../../services/api/AuthorisationService/UserActions/RegisterNewUser";

const CreateNewUser = ({ title }) => {
  const { t } = useTranslation();
  const usernameInputRef = useRef();
  const passwordInputRef = useRef();
  const [createdMessages, setCreatedMessages] = useState("");

  const registerUser = (values, props) => {
    const enteredUsername = usernameInputRef.current.value;
    const enteredPassword = passwordInputRef.current.value;
    RegisterNewUser({ enteredUsername, enteredPassword }).then((data) => {
      if (data.successful) {
        setCreatedMessages("User " + enteredUsername + " is Created.");
      } else {
        setCreatedMessages(enteredUsername + " is already exists.");
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
    <div className="newUser">
      <Sidebar items="user" />
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
            initialValues={addUserInitialValues}
            validationSchema={addUserValidationSchema}
            onSubmit={registerUser}
          >
            {(props) => (
              <Form>
                <div className="formInput">
                  <label>{t("userPage:username")}</label>
                  <Field
                    as={TextField}
                    name="username"
                    size="small"
                    variant="standard"
                    type="text"
                    inputRef={usernameInputRef}
                    helperText={<ErrorMessage name="username" />}
                  />
                  <label>{t("userPage:password")}</label>
                  <Field
                    as={TextField}
                    name="password"
                    size="small"
                    variant="standard"
                    type="password"
                    inputRef={passwordInputRef}
                    helperText={<ErrorMessage name="password" />}
                  />
                  <label>{t("userPage:confirmationPassword")}</label>
                  <Field
                    as={TextField}
                    name="confirmationPassword"
                    size="small"
                    variant="standard"
                    type="password"
                    helperText={<ErrorMessage name="confirmationPassword" />}
                  />
                  <br />
                  <br />
                  <Button
                    variant="contained"
                    type="submit"
                    disabled={props.isSubmitting}
                    endIcon={<SendIcon />}
                  >
                    {props.isSubmitting ? "Creating ..." : t("userPage:button")}
                  </Button>{" "}
                  {!props.isSubmitting && (
                    <Link to="/users" style={{ textDecoration: "none" }}>
                      <Button variant="outlined" type="submit">
                        {t("userPage:bckButton")}
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

export default CreateNewUser;
