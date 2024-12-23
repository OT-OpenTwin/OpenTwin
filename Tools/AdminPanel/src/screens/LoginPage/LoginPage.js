import "./Login.css";
import React, { useState, useRef } from "react";
import { Navigate } from "react-router-dom";
import { useTranslation } from "react-i18next";

import { Formik, Field, Form, ErrorMessage } from "formik";
import { loginInitialValues } from "./validations/initial-values";
import { loginValidationSchema } from "./validations/validation-schema";

import InfoModal from "../../components/Modal/InfoModal/InfoModal";
import { Container, Row } from "react-bootstrap";
import { Alert, TextField } from "@mui/material";
import { makeStyles } from "@material-ui/styles";
import logo from "../../resources/logo/logo.png";

import AuthenticateAdmin from "../../services/api/AuthorisationService/AuthenticateAdmin/AuthenticateAdmin";
import GetAuthServiceAddress from "../../services/api/GetServiceAddress/GetAuthServiceAddress";
import GetGlobalDirServiceAddress from "../../services/api/GetServiceAddress/GetGlobalDirServiceAddress";

export function setUserData(username, password) {
  sessionStorage.setItem("username", JSON.stringify(username));
  sessionStorage.setItem("password", JSON.stringify(password));
}

export function setSessionServiceData(sessionservice) {
  sessionStorage.setItem("sessionservice", JSON.stringify(sessionservice));
}

export function getUserData() {
  const userString = sessionStorage.getItem("username");
  const passString = sessionStorage.getItem("password");
  const userData = [JSON.parse(userString), JSON.parse(passString)];
  return userData;
}

const LoginPage = (props) => {
  const { t } = useTranslation();
  const [errorMessages, setErrorMessages] = useState(null);
  const [isSubmitted, setIsSubmitted] = useState(false);

  const sessionserviceInputRef = useRef();
  const usernameInputRef = useRef();
  const passwordInputRef = useRef();

  const [serverErrPopup, setServerErrPopup] = React.useState(false);

  const handleServerErrPopupClose = () => {
    setServerErrPopup(false);
  };

  const submitHandler = () => {
    const error = "Invalid username or password";
    const enteredSessionservice = sessionserviceInputRef.current.value;
    const enteredUsername = usernameInputRef.current.value;
    const enteredPassword = passwordInputRef.current.value;
    setUserData(enteredUsername, enteredPassword);
    setSessionServiceData(enteredSessionservice);

    // Get the authorization service address from the global session service
    GetAuthServiceAddress().then((address) => {
      if (typeof address === "undefined")
      {
          setServerErrPopup(true);
      }
      else
      {
          sessionStorage.setItem("authServiceAddress", address);

          GetGlobalDirServiceAddress().then((address) => {

            sessionStorage.setItem("globalDirServiceAddress", address);

            // Check the admin user credentials
            AuthenticateAdmin({ enteredUsername, enteredPassword }).then((data) => {
              if (data.successful === true) {
                setIsSubmitted(true);
                props.setLoggedIn(true);
                props.setToken(true);
              } else if (data.successful === false) {
                setErrorMessages({ name: "unameOrPassword", message: error });
              } else {
                setServerErrPopup(true);
              }
            });
          });  
      }
    });
  };

  const useStyles = makeStyles((theme) => ({
    root: {
      "& .css-u6loev-MuiFormHelperText-root": {
        color: "#ff0000",
      },
      "& .css-1d1r5q-MuiFormHelperText-root": {
        color: "#ff0000",
      },
    },
  }));

  const renderErrorMessage = (name) =>
    name === errorMessages.name && (
      <div className="error">{errorMessages.message}</div>
    );

  const classes = useStyles();

  return (
    <>
      <div className="Login">
        <Container>
          <InfoModal
            open={serverErrPopup}
            handleClose={handleServerErrPopupClose}
            dialogTitle={t("loginPage:serverErrPopupTitle")}
            dialogContent={t("loginPage:serverErrPopupContent")}
            dialogButton={t("loginPage:serverErrPopupButton")}
          />
          <Row>
            <div className="col-lg-3 col-md-2"></div>
            <div className="col-lg-6 col-md-8 login-box">
              <div className="col-lg-12 login-title">
                <img
                  src={logo}
                  width="200"
                  height="200"
                  alt="OT-Logo"
                  className="rounded"
                />
              </div>
              <div className="col-lg-12 login-title">
                {t("loginPage:title")}
              </div>
              <div className="col-lg-12 login-form">
                <div className="col-lg-12 login-form">
                  <Formik
                    initialValues={loginInitialValues}
                    validationSchema={loginValidationSchema}
                    onSubmit={submitHandler}
                  >
                    <Form>
                      <div className="form-group">
                        <label className="form-control-label">
                          {t("loginPage:sessionservice")}
                        </label>
                        <br />
                        <Field
                          as={TextField}
                          fullWidth
                          className={classes.root}
                          name="sessionservice"
                          size="medium"
                          variant="standard"
                          type="text"
                          inputRef={sessionserviceInputRef}
                          helperText={<ErrorMessage name="sessionservice" />}
                        />
                      </div>
                      <div className="form-group">
                        <label className="form-control-label">
                          {t("loginPage:userName")}
                        </label>
                        <br />
                        <Field
                          as={TextField}
                          fullWidth
                          className={classes.root}
                          name="username"
                          size="medium"
                          variant="standard"
                          type="text"
                          inputRef={usernameInputRef}
                          helperText={<ErrorMessage name="username" />}
                        />
                      </div>
                      <div className="form-group">
                        <label className="form-control-label">
                          {t("loginPage:password")}
                        </label>
                        <br />
                        <Field
                          as={TextField}
                          fullWidth
                          name="password"
                          size="small"
                          variant="standard"
                          type="password"
                          className={classes.root}
                          inputRef={passwordInputRef}
                          helperText={<ErrorMessage name="password" />}
                        />
                      </div>
                      <div className="col-lg-12 loginbttm">
                        {errorMessages && (
                          <div className="col-lg-12 login-btm login-text">
                            <Alert severity="error">
                              {renderErrorMessage("unameOrPassword")}
                            </Alert>
                          </div>
                        )}
                        <div className="login-btm login-button">
                          <button
                            type="submit"
                            className="btn btn-outline-primary"
                          >
                            {t("loginPage:login")}
                          </button>
                        </div>
                        {isSubmitted && (
                          <Navigate to="/landingPage" replace={true} />
                        )}
                      </div>
                    </Form>
                  </Formik>
                </div>
              </div>
            </div>
            <div className="col-lg-3 col-md-2"></div>
          </Row>
        </Container>
      </div>
    </>
  );
};

export default LoginPage;
