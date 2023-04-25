import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function ChangeUsername(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "ChangeUsername",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    OldUsername: props.oldUsername,
    NewUsername: props.newUsername,
  };

  return authRequest(data);
}

export default ChangeUsername;
