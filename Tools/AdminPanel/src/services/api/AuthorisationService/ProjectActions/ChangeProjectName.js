import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function ChangeProjectName(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "ChangeProjectName",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    ProjectName: props.enteredOldProjectName,
    NewProjectName: props.enteredNewProjectName,
  };

  return authRequest(data);
}

export default ChangeProjectName;
