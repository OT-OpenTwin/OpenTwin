import * as Yup from "yup";

export const addGroupValidationSchema = Yup.object().shape({
  groupName: Yup.string().required("Input required!"),
});

export const changeGroupNameValidationSchema = Yup.object().shape({
  oldGroup: Yup.string().required("Input required!"),
  newGroup: Yup.string().required("Input required!"),
});
