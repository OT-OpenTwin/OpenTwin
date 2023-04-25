import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function ChangePassword(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "ChangePassword",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    Username: props.username,
    NewPassword: props.newPassword,
  };

  return authRequest(data);
}

export default ChangePassword;
