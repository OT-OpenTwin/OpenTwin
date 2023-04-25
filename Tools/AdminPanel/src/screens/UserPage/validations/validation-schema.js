import * as Yup from "yup";

export const addUserValidationSchema = Yup.object().shape({
  username: Yup.string().required("Input required!"),
  password: Yup.string().required("Input required!"),
  confirmationPassword: Yup.string()
    .required("Input required!")
    .oneOf([Yup.ref("password"), null], "Passwords must match!"),
});

export const changeUsernameValidationSchema = Yup.object().shape({
  oldUsername: Yup.string().required("Input required!"),
  newUsername: Yup.string().required("Input required!"),
});

export const changePasswordValidationSchema = Yup.object().shape({
  username: Yup.string().required("Input required!"),
  newPassword: Yup.string().required("Input required!"),
  confirmationPassword: Yup.string()
    .required("Input required!")
    .oneOf([Yup.ref("newPassword"), null], "Passwords must match!"),
});
