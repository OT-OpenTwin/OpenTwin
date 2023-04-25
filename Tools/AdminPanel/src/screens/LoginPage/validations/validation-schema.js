import * as Yup from "yup";

export const loginValidationSchema = Yup.object().shape({
  username: Yup.string().required("Input required!"),
  password: Yup.string().required("Input required!"),
});