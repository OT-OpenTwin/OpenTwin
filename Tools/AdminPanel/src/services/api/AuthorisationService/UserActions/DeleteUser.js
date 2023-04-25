import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function DeleteUser(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "DeleteUser",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    UsernameToDelete: props.usernameToDelete,
  };

  return authRequest(data);
}

export default DeleteUser;
