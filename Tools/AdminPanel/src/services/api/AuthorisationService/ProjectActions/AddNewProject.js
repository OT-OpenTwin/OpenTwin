import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function AddNewProject(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "CreateProject",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    ProjectName: props.enteredProjectName,
  };

  return authRequest(data);
}

export default AddNewProject;
