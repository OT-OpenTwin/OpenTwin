import "./style/ChangeUserPassword.scss";
import { useState, useRef, useContext } from "react";
import { useTranslation } from "react-i18next";
import { Link, useNavigate } from "react-router-dom";

import { Formik, Field, Form, ErrorMessage } from "formik";
import { UserContext } from "../../components/context/userDetails";
import { changePasswordValidationSchema } from "./validations/validation-schema";

import Sidebar from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import { Alert, TextField, Button } from "@mui/material";
import SendIcon from "@mui/icons-material/Send";

import ChangePassword from "../../services/api/AuthorisationService/UserActions/ChangePassword";

const ChangeUserPassword = () => {
  const { t } = useTranslation();
  const navigate = useNavigate();

  const selectedUser = useContext(UserContext);

  const usernameInputRef = useRef();
  const newPasswordInputRef = useRef();
  const [changePasswordMessages, setChangePasswordMessages] = useState("");

  const initialValues = {
    username: selectedUser.userDetails,
    newPassword: "",
    confirmationPassword: "",
  };

  const SendPasswordChangeRequest = (values, props) => {
    const username = usernameInputRef.current.value;
    const newPassword = newPasswordInputRef.current.value;

    ChangePassword({ username, newPassword }).then((response) => {
      if (response.successful === true) {
        setChangePasswordMessages("Password changed.");
      } else {
        setChangePasswordMessages(response.description);
      }
    });
    console.log(values);
    console.log(props);
    setTimeout(() => {
      props.resetForm();
      props.setSubmitting(false);
      navigate("/users");
    }, 2000);
  };

  return (
    <div className="userPassword">
      <Sidebar items="user" />
      <div className="changeContainer">
        <Navbar />
        <br />
        <br />
        <br />
        <Formik
          initialValues={initialValues}
          validationSchema={changePasswordValidationSchema}
          onSubmit={SendPasswordChangeRequest}
        >
          {(props) => (
            <Form>
              <div className="top">
                <div className="box">
                  <h1 className="itemTitle">{t("userPage:changePassword")}</h1>
                  <div className="item">
                    <div className="details">
                      <div> {t("userPage:username")}</div>
                      <Field
                        as={TextField}
                        size="small"
                        variant="standard"
                        type="text"
                        name="username"
                        inputRef={usernameInputRef}
                        helperText={<ErrorMessage name="username" />}
                        InputProps={{
                          readOnly: true,
                        }}
                      />
                      <div> {t("userPage:newPassword")}</div>
                      <Field
                        as={TextField}
                        size="small"
                        variant="standard"
                        type="password"
                        name="newPassword"
                        inputRef={newPasswordInputRef}
                        helperText={<ErrorMessage name="newPassword" />}
                      />
                      <div>{t("userPage:confirmationPassword")}</div>
                      <Field
                        as={TextField}
                        name="confirmationPassword"
                        size="small"
                        variant="standard"
                        type="password"
                        helperText={
                          <ErrorMessage name="confirmationPassword" />
                        }
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
                          : t("userPage:changeButton")}
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
                      {changePasswordMessages &&
                        (props.isSubmitting ? (
                          <Alert severity="info">
                            {changePasswordMessages}
                          </Alert>
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

        <div className="bottom"></div>
      </div>
    </div>
  );
};

export default ChangeUserPassword;
