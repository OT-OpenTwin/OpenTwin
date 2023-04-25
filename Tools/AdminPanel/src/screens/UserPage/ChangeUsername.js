import "./style/ChangeUserUsername.scss";
import { useState, useRef, useContext } from "react";
import { useTranslation } from "react-i18next";
import { Link, useNavigate } from "react-router-dom";

import { Formik, Field, Form, ErrorMessage } from "formik";
import { UserContext } from "../../components/context/userDetails";
import { changeUsernameValidationSchema } from "./validations/validation-schema";

import Sidebar from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import { Alert, TextField, Button } from "@mui/material";
import SendIcon from "@mui/icons-material/Send";

import ChangeUsername from "../../services/api/AuthorisationService/UserActions/ChangeUsername";

const ChangeUserUsername = () => {
  const { t } = useTranslation();
  const navigate = useNavigate();

  const selectedUser = useContext(UserContext);

  const oldUsernameInputRef = useRef();
  const newUsernameInputRef = useRef();
  const [changeUsernameMessage, setChangeUsernameMessage] = useState("");

  const initialValues = {
    oldUsername: selectedUser.userDetails,
    newUsername: "",
  };

  const SendUsernameChangeRequest = (values, props) => {
    const oldUsername = oldUsernameInputRef.current.value;
    const newUsername = newUsernameInputRef.current.value;

    ChangeUsername({ oldUsername, newUsername }).then((response) => {
      if (response.successful === true) {
        setChangeUsernameMessage("User changed.");
      } else {
        setChangeUsernameMessage(response.description);
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
    <div className="userUname">
      <Sidebar items="user" />
      <div className="changeContainer">
        <Navbar />
        <br />
        <br />
        <br />
        <Formik
          initialValues={initialValues}
          validationSchema={changeUsernameValidationSchema}
          onSubmit={SendUsernameChangeRequest}
        >
          {(props) => (
            <Form>
              <div className="top">
                <div className="box">
                  <h1 className="itemTitle">{t("userPage:changeUsername")}</h1>
                  <div className="item">
                    <div className="details">
                      <div> {t("userPage:oldUsername")}</div>
                      <Field
                        as={TextField}
                        size="small"
                        variant="standard"
                        type="text"
                        name="oldUsername"
                        inputRef={oldUsernameInputRef}
                        helperText={<ErrorMessage name="oldUsername" />}
                        InputProps={{
                          readOnly: true,
                        }}
                      />
                      <div> {t("userPage:newUsername")}</div>
                      <Field
                        as={TextField}
                        size="small"
                        variant="standard"
                        type="text"
                        name="newUsername"
                        inputRef={newUsernameInputRef}
                        helperText={<ErrorMessage name="newUsername" />}
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
                      {changeUsernameMessage &&
                        (props.isSubmitting ? (
                          <Alert severity="info">{changeUsernameMessage}</Alert>
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

export default ChangeUserUsername;
