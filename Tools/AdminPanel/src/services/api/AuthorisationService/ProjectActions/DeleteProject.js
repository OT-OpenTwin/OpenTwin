import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function DeleteProject(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "RemoveProject",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    ProjectName: props.projectNameToRemove,
  };

  return authRequest(data);
}

export default DeleteProject;
