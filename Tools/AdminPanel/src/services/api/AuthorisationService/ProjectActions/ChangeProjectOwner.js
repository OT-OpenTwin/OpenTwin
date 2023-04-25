import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function ChangeProjectOwner(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "ChangeProjectOwner",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    ProjectName: props.projectName,
    NewProjectOwner: props.ownerUsername,
  };

  return authRequest(data);
}

export default ChangeProjectOwner;
