import * as Yup from "yup";

export const addProjectValidationSchema = Yup.object().shape({
  projectName: Yup.string().required("Input required!"),
});

export const changeProjectNameValidationSchema = Yup.object().shape({
  oldProject: Yup.string().required("Input required!"),
  newProject: Yup.string().required("Input required!"),
});
