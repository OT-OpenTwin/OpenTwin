import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function ChangeGroupName(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "ChangeGroupName",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    GroupName: props.enteredOldGroupName,
    NewGroupName: props.enteredNewGroupName,
  };

  return authRequest(data);
}

export default ChangeGroupName;
